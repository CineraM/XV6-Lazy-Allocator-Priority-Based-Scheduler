#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define STRING_SIZE 100
#define N 500
// store strings, eval strings
char file_contents[N][STRING_SIZE];
char eval[N][STRING_SIZE];
int eval_bool[N];

// buffer variables
char buff;
char temp_str[STRING_SIZE];

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
            if (file_contents[i][z] >= 48 && file_contents[i][z] <= 57) break;  // digits
            if (file_contents[i][z] >= 65 && file_contents[i][z] <= 90) break;  // cap
            if (file_contents[i][z] >= 97 && file_contents[i][z] <= 122) break; // lower
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
            }
            eval[eidx][z] = temp_str[z];
        }
        eidx++; // increase index for next string
    }
}

void bubble_sort(int n)
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
                if(eval_bool[j] && !eval_bool[i])
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
        bubble_sort(num_of_lines);


        // print contents
        for (int i = 0; i <= num_of_lines; i++)
        {
            // printf(1, "%c", file_contents[i][0]);
            printf(1, "%s", file_contents[i]);
        } 

        close(fd);
    }

    exit();
}
