#include "commands.h"
#include "common.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

void update_log_file(char hunt[], char op[], char tid[], int failed) {
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt,"logged_hunt.txt");

    FILE *file;
    if((file = fopen(path,"a")) == NULL) {
        printf("Error at opening log file!\n");
        return;
    }

    char msg[PATH_SIZE] = "";

    if(failed==1) {
        if(strcmp(op,"--list")==0) {
            strcat(msg,"Listed hunt ");
            strcat(msg,hunt);
            strcat(msg," -> failed!");
        }
        else if(strcmp(op,"--add")==0) {
            strcat(msg,"Added treasure ");
            strcat(msg,tid);
            strcat(msg," to hunt ");
            strcat(msg,hunt);
            strcat(msg," -> failed!");
        }
        else if(strcmp(op,"--view")==0) {
            strcat(msg,"Viewed treasure ");
            strcat(msg,tid);
            strcat(msg," of hunt ");
            strcat(msg,hunt);
            strcat(msg," -> failed!");
        }
        else if(strcmp(op,"--remove_treasure")==0) {
            strcat(msg,"Removed treasure ");
            strcat(msg,tid);
            strcat(msg," of hunt ");
            strcat(msg,hunt);
            strcat(msg," -> failed!");
        }
        else if(strcmp(op,"--remove_hunt")==0) {
            strcat(msg,"Removed hunt ");
            strcat(msg,hunt);
            strcat(msg," -> failed!");
        }
    } else {
        if(strcmp(op,"--list")==0) {
            strcat(msg,"Listed hunt ");
            strcat(msg,hunt);
        }
        else if(strcmp(op,"--add")==0) {
            strcat(msg,"Added treasure ");
            strcat(msg,tid);
            strcat(msg," to hunt ");
            strcat(msg,hunt);
        }
        else if(strcmp(op,"--view")==0) {
            strcat(msg,"Viewed treasure ");
            strcat(msg,tid);
            strcat(msg," of hunt ");
            strcat(msg,hunt);
        }
        else if(strcmp(op,"--remove_treasure")==0) {
            strcat(msg,"Removed treasure ");
            strcat(msg,tid);
            strcat(msg," of hunt ");
            strcat(msg,hunt);
        }
        else if(strcmp(op,"--remove_hunt")==0) {
            strcat(msg,"Removed hunt ");
            strcat(msg,hunt);
        } else if(strcmp(op,"--list_hunts")==0) {
            strcat(msg,"Listed hunts !");
        }
    }

    strcat(msg,"\n");
    fprintf(file, "%s", msg); 
    fclose(file);
}

void create_treasure(char *hunt) {
    treasure_t t;
 
    printf("Please enter the following data:\n");
 
    printf("Treasure ID:");
    scanf("%s", t.id);
 
    printf("Username:");
    scanf("%s", t.user);
 
    printf("Latitude:");
    scanf("%lf", &t.lat);
 
    printf("Longitude:");
    scanf("%lf", &t.lon);
 
    printf("Clue Text:");
    getchar();
    fgets(t.clue,sizeof(t.clue),stdin);
    t.clue[strlen(t.clue)-1] = '\0';
 
    printf("Value:");
    scanf("%d", &t.value);
 
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt,"treasure.bin");
 
    int file;
    if((file = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777))==0) {
        printf("error at opening file\n");
        update_log_file(hunt,"--add",t.id,1);
        return;
    }
 
    if(write(file, &t, sizeof(treasure_t)) != sizeof(treasure_t)) {
        printf("error at write\n");
        update_log_file(hunt,"--add",t.id,1);
        close(file);
        return;
    }

    close(file);
    update_log_file(hunt,"--add",t.id,0);
}

void view_treasure(char hunt_id[], char treasure_id[]) {
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");
 
    int file;
    if((file = open(path, O_RDONLY, 0777)) == 0) {
        printf("Error at opening file\n");
        update_log_file(hunt_id,"--view","",1);
        return;
    } 
 
    treasure_t t;
    if(read(file,&t,sizeof(treasure_t))!= sizeof(treasure_t)) {
        printf("error at reading");
        update_log_file(hunt_id,"--view_treasure",treasure_id,1);
        close(file);
        return;
    }

    if(strcmp(treasure_id,t.id)==0) {
        printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n",
               t.id,t.user,t.lat,t.lon,t.clue,t.value);
        close(file);
        update_log_file(hunt_id,"--view_treasure",treasure_id,1);
        return;
    }

    while(read(file,&t,sizeof(treasure_t))== sizeof(treasure_t)) {
        if(strcmp(treasure_id,t.id)==0) {
            printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n",
                   t.id,t.user,t.lat,t.lon,t.clue,t.value);
            close(file);
            return;
        }
    }

    close(file);
    update_log_file(hunt_id,"--view_treasure",treasure_id,0);
}

void list_treasures(char hunt_id[]) {
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");
 
    struct stat st;
    stat(path, &st);
    int size = st.st_size;
    time_t lastmod = st.st_mtime;
 
    treasure_t t;
    int file;
 
    if((file = open(path, O_RDONLY, 0777)) == 0) {
        printf("Error at opening file\n");
        update_log_file(hunt_id,"--list","",1);
        return;
    } 
 
    printf("Hunt: %s\nSize of file:%d\nLast modification:%s\n",hunt_id,size,ctime(&lastmod));
 
    while(read(file,&t,sizeof(treasure_t))== sizeof(treasure_t)) {
        printf("Treasure ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n\n",
               t.id,t.user,t.lat,t.lon,t.clue,t.value);
    }
 
    close(file);
    update_log_file(hunt_id,"--list","",0);
}

void delete_hunt(char hunt_id[]) {
    static char path[PATH_SIZE], path2[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");

    if(unlink(path)!=0) {
        printf("Error at unlinking binary file\n");
        update_log_file(hunt_id,"--remove_hunt","",1);
        return;
    }

    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"logged_hunt.txt");
    snprintf(path2, sizeof(path),"hunts/%s%s","logged_hunt_",hunt_id);

    update_log_file(hunt_id,"--remove_hunt","",0);

    if(unlink(path)!=0) {
        printf("Error at unlinking log file\n");
        update_log_file(hunt_id,"--remove_hunt","",1);
        return;
    }

    if(unlink(path2)!=0) {
        printf("Error at unlinking root log file\n");
        update_log_file(hunt_id,"--remove_hunt","",1);
        return;
    }

    if(rmdir(hunt_id)!=0) {
        printf("Error at deleting directory\n");
        update_log_file(hunt_id,"--remove_hunt","",1);
    }
}

void delete_treasure(char hunt_id[], char treasure_id[]) {
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");
 
    int file;
    if((file = open(path, O_RDONLY, 0777)) == 0) {
        printf("Error at opening file\n");
        update_log_file(hunt_id,"--remove_treasure","",1);
        return;
    } 
 
    treasure_t t;
    FILE *temp = tmpfile();
    if (!temp) {
        printf("Error creating temporary file\n");
        close(file);
        update_log_file(hunt_id,"--remove_treasure",treasure_id,1);
        return;
    }
 
    int found = 0;
    while(read(file,&t,sizeof(treasure_t))== sizeof(treasure_t)) {
        if(strcmp(treasure_id,t.id)!=0) {
            if(fwrite(&t, sizeof(treasure_t), 1, temp) != 1) {
                printf("Error writing to temporary file\n");
                close(file);
                fclose(temp);
                update_log_file(hunt_id,"--remove_treasure",treasure_id,1);
                return;
            }
        } else {
            found = 1;
        }
    }
 
    if(!found) {
        printf("Treasure not found\n");
        close(file);
        fclose(temp);
        update_log_file(hunt_id,"--remove_treasure",treasure_id,1);
        return;
    }
 
    close(file);
    
    if((file = open(path, O_WRONLY | O_TRUNC, 0777)) == 0) {
        printf("Error at opening file for writing\n");
        fclose(temp);
        update_log_file(hunt_id,"--remove_treasure",treasure_id,1);
        return;
    }
 
    rewind(temp);
    while(fread(&t, sizeof(treasure_t), 1, temp) == 1) {
        if(write(file, &t, sizeof(treasure_t)) != sizeof(treasure_t)) {
            printf("Error writing back to original file\n");
            close(file);
            fclose(temp);
            update_log_file(hunt_id,"--remove_treasure",treasure_id,1);
            return;
        }
    }
 
    close(file);
    fclose(temp);
    update_log_file(hunt_id,"--remove_treasure",treasure_id,0);
}

int create_bin_file(char dir[]) {
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",dir,"treasure.bin");
 
    int file;
    if((file = open(path, O_WRONLY | O_CREAT, 0777))==0) {
        printf("Error at creating binary file\n");
        return 0;
    }
    close(file);
    return 1;
}

int create_log_file(char dir[]) {
    static char path[PATH_SIZE];
    snprintf(path, sizeof(path),"hunts/%s/%s",dir,"logged_hunt.txt");
 
    FILE *file;
    if((file = fopen(path,"w"))==NULL) {
        printf("Error at creating log file\n");
        return 0;
    }
    fclose(file);
    return 1;
}

void list_hunts(char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    
    if((dir = opendir(path)) == NULL) {
        printf("Error at opening directory\n");
        update_log_file("","--list_hunts","",1);
        return;
    }
    
    printf("Available hunts:\n");
    while((entry = readdir(dir)) != NULL) {
        char full_path[PATH_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        if(stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("%s\n", entry->d_name);
            }
        }
    }
    
    closedir(dir);
    update_log_file("","--list_hunts","",0);
}