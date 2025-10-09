#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include <unistd.h>    
#include <sys/stat.h>  
#include <string.h>    

#define MAX_FILES 1000
#define MAX_NAME 256

#define DIR_CLR "\x1b[34m"
#define EXEC_CLR "\x1b[32m"
#define LINK_CLR "\x1b[36m"
#define NO_CLR "\x1b[0m"

#define IS_DIR(mode) (((mode) & 0170000) == 0040000)
#define IS_LNK(mode) (((mode) & 0170000) == 0120000)

#define IS_READABLE(mode, user) ((mode & (user ? 0400 : (user == 1 ? 0040 : 0004))) != 0)
#define IS_WRITABLE(mode, user) ((mode & (user ? 0200 : (user == 1 ? 0020 : 0002))) != 0)
#define IS_EXECUTABLE(mode, user) ((mode & (user ? 0100 : (user == 1 ? 0010 : 0001))) != 0)

struct FileData {
    char name[MAX_NAME];
    struct stat info;
};

void get_permissions(unsigned int mode, char *perm) {
    if (IS_DIR(mode)) perm[0] = 'd';
    else if (IS_LNK(mode)) perm[0] = 'l';
    else perm[0] = '-';

    perm[1] = IS_READABLE(mode, 1) ? 'r' : '-';
    perm[2] = IS_WRITABLE(mode, 1) ? 'w' : '-';
    perm[3] = IS_EXECUTABLE(mode, 1) ? 'x' : '-';

    perm[4] = IS_READABLE(mode, 2) ? 'r' : '-';
    perm[5] = IS_WRITABLE(mode, 2) ? 'w' : '-';
    perm[6] = IS_EXECUTABLE(mode, 2) ? 'x' : '-';

    perm[7] = IS_READABLE(mode, 3) ? 'r' : '-';
    perm[8] = IS_WRITABLE(mode, 3) ? 'w' : '-';
    perm[9] = IS_EXECUTABLE(mode, 3) ? 'x' : '-';
    perm[10] = '\0';
}

const char* get_color(unsigned int mode) {
    if (IS_DIR(mode)) return DIR_CLR;
    if (IS_LNK(mode)) return LINK_CLR;
    if (IS_EXECUTABLE(mode, 1)) return EXEC_CLR;
    return NO_CLR;
}

void print_long_format(struct FileData *files, int count, const char *path) {
    long total = 0;
    for (int i = 0; i < count; i++) {
        total += files[i].info.st_blocks;
    }
    printf("total %ld\n", total / 2);

    for (int i = 0; i < count; i++) {
        struct stat *st = &files[i].info;
        char perm[11];
        get_permissions(st->st_mode, perm);

        struct passwd *pwd = getpwuid(st->st_uid);
        struct group *grp = getgrgid(st->st_gid);

        char time_str[64];
        struct tm *tm = localtime(&st->st_mtime);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm);

        printf("%s %ld %s %s %ld %s ", perm, (long)st->st_nlink,
               pwd ? pwd->pw_name : "unknown",
               grp ? grp->gr_name : "unknown",
               (long)st->st_size, time_str);

        printf("%s%s%s", get_color(st->st_mode), files[i].name, NO_CLR);

        if (IS_LNK(st->st_mode)) {
            char link_target[1024];
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, files[i].name);
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf(" -> %s", link_target);
            }
        }
        printf("\n");
    }
}

void print_simple(struct FileData *files, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s%s%s\n", get_color(files[i].info.st_mode), files[i].name, NO_CLR);
    }
}

int read_directory(const char *path, struct FileData *files, int show_all) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Cannot open directory");
        return -1;
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL && count < MAX_FILES) {
        if (!show_all && entry->d_name[0] == '.') continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (lstat(full_path, &st) == -1) continue;

        files[count].info = st;
        strncpy(files[count].name, entry->d_name, MAX_NAME - 1);
        files[count].name[MAX_NAME - 1] = '\0';
        count++;
    }

    closedir(dir);
    return count;
}

int main(int argc, char *argv[]) {
    int show_all = 0;
    int long_format = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
            for (int j = 1; j < strlen(argv[i]); j++) {
                if (argv[i][j] == 'a') show_all = 1;
                if (argv[i][j] == 'l') long_format = 1;
            }
        }
    }

    char *path = ".";
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            path = argv[i];
            break;
        }
    }

    struct FileData files[MAX_FILES];
    int count = read_directory(path, files, show_all);

    if (count < 0) return 1;

    if (long_format) {
        print_long_format(files, count, path);
    } else {
        print_simple(files, count);
    }

    return 0;
}
