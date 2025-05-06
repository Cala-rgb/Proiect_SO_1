#include "commands.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

typedef struct
{
    char id[25];
    char user[25];
    double lat, lon;
    char clue[25];
    int value;
} treasure;

void updateLogFile(char hunt[], char op[], char tid[], int failed) {

    static char path[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt,"logged_hunt.txt");

    FILE *file;

    if((file = fopen(path,"a")) == NULL) {
        printf("Error at opening log file!\n");
        return ;
    }

    char msg[128] = "";

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

    fprintf(file,msg); 
    fclose(file);

}
 
 
void createTreasure(char *hunt)
{
    treasure t;
 
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
 
 
 
    static char path[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt,"treasure.bin");
 
    int file;
    if((file = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777))==0) {
        printf("error at opening file\n");
        updateLogFile(hunt,"--add",t.id,1);
    }
 
    if(write(file, &t, sizeof(treasure)) != sizeof(treasure)) {
        printf("error at write\n");
        updateLogFile(hunt,"--add",t.id,1);
    }

    updateLogFile(hunt,"--add",t.id,0);
 
}
 
void view_treasure(char hunt_id[], char treasure_id[]) {
    static char path[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");
 
    int file;

    if((file = open(path, O_RDONLY, 0777)) == 0) {
        printf("Error at opening file\n");
        updateLogFile(hunt_id,"--view","",1);
    } 
 
    treasure t;
 
    if(read(file,&t,sizeof(treasure))!= sizeof(treasure)) {
        printf("error at reading");
        updateLogFile(hunt_id,"--view_treasure",treasure_id,1);
    } else {
        //printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n",t.id,t.user,t.lat,t.lon,t.clue,t.value);
        if(strcmp(treasure_id,t.id)==0) {
            printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n",t.id,t.user,t.lat,t.lon,t.clue,t.value);
            updateLogFile(hunt_id,"--view_treasure",treasure_id,1);
        } else {
            while(read(file,&t,sizeof(treasure))== sizeof(treasure)) {
                if(strcmp(treasure_id,t.id)==0) {
                    printf("ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n",t.id,t.user,t.lat,t.lon,t.clue,t.value);
                    return ;
                }
            }
        }
    }

    updateLogFile(hunt_id,"--view_treasure",treasure_id,0);
}
 
void list(char hunt_id[]) {
 
    static char path[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");
 
    struct stat st;
    stat(path, &st);
    int size = st.st_size;
    time_t lastmod = st.st_mtime;
 
    treasure t;

    int file;
 
    if((file = open(path, O_RDONLY, 0777)) == 0) {
        printf("Error at opening file\n");
        updateLogFile(hunt_id,"--list","",1);
    } 
 
    printf("Hunt: %s\nSize of file:%d\nLast modification:%s\n",hunt_id,size,ctime(&lastmod));
 
    while(read(file,&t,sizeof(treasure))== sizeof(treasure)) {
        printf("Treasure ID:%s\nUser:%s\nLatitude:%lf\nLongitude:%lf\nClue:%s\nValue:%d\n\n",t.id,t.user,t.lat,t.lon,t.clue,t.value);
    }
 
    updateLogFile(hunt_id,"--list","",0);
 
}
 
void delete_dir(char hunt_id[]) {
 
    static char path[128],path2[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");

    if(sizeof(path)>0) {
        if(unlink(path)!=0) {
            printf("Error at unlinking binary file\n");
            updateLogFile(hunt_id,"--remove_hunt","",1);
        }
    }

    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"logged_hunt.txt");
    snprintf(path2, sizeof(path),"hunts/%s%s","logged_hunt_",hunt_id);

    updateLogFile(hunt_id,"--remove_hunt","",0);

    if(unlink(path)!=0) {
        printf("Error at unlinking log file\n");
        updateLogFile(hunt_id,"--remove_hunt","",1);
    }

    if(unlink(path2)!=0) {
        printf("Error at unlinking root log file\n");
        updateLogFile(hunt_id,"--remove_hunt","",1);
    }

    if(rmdir(hunt_id)!=0) {
        printf("Error at deleting directory\n");
        updateLogFile(hunt_id,"--remove_hunt","",1);
    }
 
    

}
 
void delete_treasure(char hunt_id[], char treasure_id[]) {
    static char path[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",hunt_id,"treasure.bin");
 
    int file;
    if((file = open(path, O_RDWR, 0777)) == 0) {
        printf("error at opening file\n");
        updateLogFile(hunt_id,"--remove_treasure",treasure_id,1);
    }
 
    treasure t;
 
    int index=0;
 
    while(read(file,&t,sizeof(treasure))== sizeof(treasure)) {
        if(strcmp(treasure_id,t.id)==0) {
            break;
        }
        index++;
    }

    int current_size = lseek(file, 0, SEEK_END);
 
    int pos = index*sizeof(treasure);

    if(pos>=current_size) {
        printf("Treasure not found!\n");
        updateLogFile(hunt_id,"--remove_treasure",treasure_id,1);
        return;
    }

    if(current_size >= sizeof(treasure)) {
        lseek(file,pos+sizeof(treasure),SEEK_SET);
        int curent_pos =pos+sizeof(treasure);
        while(read(file,&t,sizeof(treasure))== sizeof(treasure)) {
            curent_pos += sizeof(treasure);
            lseek(file,curent_pos-2*sizeof(treasure),SEEK_SET);
            if(write(file, &t, sizeof(treasure)) != sizeof(treasure)) {
                printf("error at write\n");
                updateLogFile(hunt_id,"--remove_treasure",treasure_id,1);
            }
            lseek(file,curent_pos,SEEK_SET);
        }
    }

    int new_size = current_size - sizeof(treasure);

    if (ftruncate(file, new_size) == -1) {
        printf("Error at truncate\n");
        updateLogFile(hunt_id,"--remove_treasure",treasure_id,1);
    }

    updateLogFile(hunt_id,"--remove_treasure",treasure_id,0);
 
}

int createBinFile(char dir[]) {
    static char path[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",dir,"treasure.bin");

    int file;
 
    if((file = open(path,O_CREAT, 0777)) == 0) {
        printf("Error at creating binary file!\n");
        return 1;
    }
    else {
        close(file);
        return 1;
    }
}

int createLogFile(char dir[]) {
    static char path[128],path2[128];
    snprintf(path, sizeof(path),"hunts/%s/%s",dir,"logged_hunt.txt");

    FILE *file;
 
    if((file = fopen(path,"w")) == NULL) {
        printf("HERE");
        printf("Error at creating log file!\n");
        return 0;
    }
    else {
        fprintf(file,"Log file for hunt %s\n",dir);
        fclose(file); 
    }

    snprintf(path2, sizeof(path2),"hunts/%s%s","logged_hunt_",dir);

    /*if((file = fopen(path2,"w")) == 0) {
        printf("Error at creating root log file!");
        return 0;
    }
    else {
        fclose(file); 
    }*/

    //printf("ajunge aici");

    if (symlink(path, path2) == -1) {
        perror("Error at linking log files!");
        return 0;
    }
    return 1;

}

int count_treasures(const char *path)
{
    int file;
    if ((file = open(path, O_RDONLY, 0777)) == 0)
    {
        printf("error at opening bin file\n");
    }
    int file_size = lseek(file, 0, SEEK_END);
    close(file);
    return file_size / sizeof(treasure);
}

void list_hunts(char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char fullpath[260];

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return ;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            char binFilePath[360];
            snprintf(binFilePath, sizeof(binFilePath), "%s/%s", fullpath, "treasure.bin");
            int noOfTreasures = count_treasures(binFilePath);
            printf("Hunt ID: %s, Number of Treasures: %d\n", entry->d_name, noOfTreasures);
        }
    }

    closedir(dir);
}