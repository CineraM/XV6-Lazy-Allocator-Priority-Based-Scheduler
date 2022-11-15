#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define STRING_SIZE 100
#define N 400
// store strings, eval strings
char file_contents[N][STRING_SIZE];
char eval[N][STRING_SIZE];
int eval_bool[N];

// buffer variables
char buff;
char temp_str[STRING_SIZE];

//sort -n variables
char nums[N][STRING_SIZE]; 
int eval_n[N];

void save_to_file(char* name, int n)
{
    int fd, i, j;

    fd = open(name, O_CREATE | O_RDWR);
    if(fd >= 0) 
    {
        printf(1, "%s created sucessfully\n", name);
    } else 
    {
        printf(1, "error: create backup file failed\n");
        exit();
    }

    int size = sizeof(file_contents[0][0]);
    for(i=0; file_contents[i][0] != '\0'; i++)
    {
        for(j=0; file_contents[i][j] != '\0'; j++)
            write(fd, &file_contents[i][j], size);
    }

    printf(1, "Successful write to %s\n", name);
    printf(1, "Printed the contents of the file with 'cat'\n");
    close(fd);
}

int read_contents(int fd)
{
    int n = 0;
    int line_len = 0;
    
    while (read(fd, &buff, 1) > 0) 
    {
        if (buff == '\n')
        {
            if(line_len == 0)
                file_contents[n][0] = '\n';
            else
                file_contents[n][line_len] = buff;
            
            n++;
            line_len = 0;
            continue;
        }

        file_contents[n][line_len] = buff;
        line_len++;
    }

    // add extra line (only if last line is not a '\n')
    if(file_contents[n][0] == '\0')
        file_contents[n][0] = '\n';
    else
    {
        int i = 0;
        while(1)
        {
            if(file_contents[n][i+1] == '\0')
            {
                if(file_contents[n][i] != '\n') file_contents[n][i+1] = '\n';
                break; 
            }
            i++;
        }
    }

    return n;
}

void truncate_strings(int num_of_lines)
{
    memset(eval_bool, 0, N);
    int i, k, z;
    int copy_idx = 0;
    int eidx = 0;
    for(i=0; i<=num_of_lines; i++)
    {
        memset(temp_str, 0, STRING_SIZE);   // reset temp buffer
        // finding alpha, or numeric character index in current string
        for(z=0; z<STRING_SIZE; z++)
        {
            // if(file_contents[i][z] != ' ')
            //     continue;
            // else
            //     break;
            if (file_contents[i][z] >= 48 && file_contents[i][z] <= 57) break;  // digits
            if (file_contents[i][z] >= 65 && file_contents[i][z] <= 90) break;  // cap
            if (file_contents[i][z] >= 97 && file_contents[i][z] <= 122) break; // lower
            if (file_contents[i][z] == '-') break;  // possible negative numbers
            if (file_contents[i][z] == '\0') break;
        }
        if(z==STRING_SIZE || file_contents[i][z] == '\0')    // if there is not a numeric || alpha char, copy all the string
        {
            for(k = 0; k<STRING_SIZE; k++) temp_str[copy_idx] = file_contents[i][copy_idx];
        }
        else // truncate the string until the desired character
        {
            copy_idx = 0;
            for(k = z; k<STRING_SIZE; k++)
            {
                temp_str[copy_idx] = file_contents[i][k];
                copy_idx++;
            }
        }
        // copy the string into eval
        for(z=0; z<STRING_SIZE;z++) 
        {
            // 0 == caps or other characters
            // 1 == lowecase
            // 2 ==  numbers
            if(z==0)
            {
                if(temp_str[0] >= 'A' && temp_str[0] <= 'Z')
                {
                    temp_str[0] += 32;
                }
                else if(temp_str[0] >= 'a' && temp_str[0] <= 'z')  
                {
                    eval_bool[i] = 1;
                }
                else if(temp_str[0] >= '0' && temp_str[0] <= '9')
                {
                    eval_bool[i] = 2;
                }
                else if(temp_str[0] == '-')
                {
                    if(temp_str[1] >= '0' && temp_str[1] <= '9')
                        eval_bool[i] = 2;
                }
            }
            eval[eidx][z] = temp_str[z];
        }
        eidx++; // increase index for next string
    }
}

// bubble sort
void sort(int n)
{
    int i, j, k, flag;
    for(i=0; i<=n; i++)
    {                 
        for(j=i+1; j<=n; j++)
        {
            flag = 0;
            int compare = strcmp(eval[i], eval[j]);
            if(compare == 0)
            {
                if(eval_bool[j] == 1 && eval_bool[i] != 1)
                    flag=1;
            }
            if(compare > 0 || flag)
            {
                // original elements
                for(k=0; k<STRING_SIZE;k++) temp_str[k] = file_contents[i][k];
                for(k=0; k<STRING_SIZE;k++) file_contents[i][k] = file_contents[j][k]; 
                for(k=0; k<STRING_SIZE;k++) file_contents[j][k] = temp_str[k];
                // truncated elements
                for(k=0; k<STRING_SIZE;k++) temp_str[k] = eval[i][k];
                for(k=0; k<STRING_SIZE;k++) eval[i][k] = eval[j][k]; 
                for(k=0; k<STRING_SIZE;k++) eval[j][k] = temp_str[k];
                // bool matrix
                int temp_bool = eval_bool[i];
                eval_bool[i] = eval_bool[j];
                eval_bool[j] = temp_bool;
            }
        }
    }
}

void sort_n(int n)
{
    truncate_strings(n);
    char str_num[STRING_SIZE];
    int i, j, k, isNum, isNeg;
    int nums_index = 0;

    // loop over the read contents
    for(i=0; eval[i][0]!='\0'; i++)
    {
        isNum=0;
        isNeg=0;
        if (eval[i][0] >= 48 && eval[i][0] <= 57) isNum=1;  // digits
        if (eval[i][0] == '-')
        {
            if (eval[i][1] >= 48 && eval[i][1] <= 57)
                isNeg=1;
        }

        // copy nums into
        if(isNeg || isNum)
        {
            j=0;
            for(k=0; k<STRING_SIZE; k++)
            {
                if(k==0) if(isNeg) continue;

                if(eval[i][1] < 48 && eval[i][1] > 57) break;
                str_num[j] = eval[i][k];
                j++;
            }
            str_num[j] = '\0';
            for(k=0; k<STRING_SIZE; k++) nums[nums_index][k] = file_contents[i][k];
            
            eval_n[nums_index] = atoi(str_num);
            if(isNeg)
                eval_n[nums_index] *= -1;

            nums_index++;
            memset(str_num, 0, STRING_SIZE);
        }
    }

    int temp_n;
    // bouble sort the numbers
    for(i=0; i<=nums_index; i++)
    {                 
        for(j=i+1; j<=nums_index; j++)
        {
            if(eval_n[i]>eval_n[j])
            {
                // original elements
                for(k=0; k<STRING_SIZE;k++) temp_str[k] = nums[i][k];
                for(k=0; k<STRING_SIZE;k++) nums[i][k] = nums[j][k]; 
                for(k=0; k<STRING_SIZE;k++) nums[j][k] = temp_str[k];

                // numerical values
                temp_n = eval_n[i];
                eval_n[i] = eval_n[j];
                eval_n[j] = temp_n;
            }
        }
    }
    // sort the original input
    sort(n);
    
    // print -n
    int flag=1;
    for(i=0; i<=nums_index; i++)
    {
        if(0<eval_n[i] && flag) // will only print once
        {
            for (int j = 0; j <= n; j++)
            {
                if(eval_bool[j] == 2) continue;
                printf(1, "%s", file_contents[j]);
            } 
            flag=0;
        }
        printf(1, "%s", nums[i]);
    }

    if(nums_index == 0 || flag)
    {
        for (int j = 0; j <= n; j++)
        {
            if(eval_bool[j] == 2) continue;
            printf(1, "%s", file_contents[j]);
        }  
    }

}


int main(int argc, char *argv[])
{
    int fd = 0;
    if(argc == 2)
    {
        if((fd = open(argv[1], 0)) < 0)
        {
            printf(1, "sort: cannot open file %s\n", argv[1]);
            exit();
        }
        int num_of_lines = read_contents(fd);
        truncate_strings(num_of_lines);
        sort(num_of_lines);

        // print contents
        for (int i = 0; i <= num_of_lines; i++)
        {
            // printf(1, "%c", file_contents[i][0]);
            printf(1, "%s", file_contents[i]);
        } 

    }
    else if(argc == 3)
    {
        if((fd = open(argv[2], 0)) < 0)
        {
            printf(1, "sort: cannot open file %s\n", argv[2]);
            exit();
        }

        if(strcmp(argv[1], "-n") == 0)
        {
            int num_of_lines = read_contents(fd);
            sort_n(num_of_lines);
        }
        if(strcmp(argv[1], "-r") == 0)
        {
            int num_of_lines = read_contents(fd);
            truncate_strings(num_of_lines);
            sort(num_of_lines);
            for (int i = num_of_lines; i >=0; i--)
            {
                printf(1, "%s", file_contents[i]);
            } 
        }
        else
        {
            printf(1, "Error: Wrong args\n");
            exit();
        }
    }
    else if(argc == 4)
    {
        if((fd = open(argv[3], 0)) < 0)
        {
            printf(1, "sort: cannot open file %s\n", argv[3]);
            exit();
        }

        if(strcmp(argv[1], "-o") == 0)
        {
            int num_of_lines = read_contents(fd);
            truncate_strings(num_of_lines);
            sort(num_of_lines);
            save_to_file(argv[2], num_of_lines);
        }
    }
    else
    {
        printf(1, "Error: Wrong args\n");
        exit();
    }

    close(fd);
    exit();
}
