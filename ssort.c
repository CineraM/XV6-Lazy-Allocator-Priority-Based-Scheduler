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

//-n variables
char nums[N][STRING_SIZE]; 
int eval_n[N];

//-M variables
char months[12][10] = { "january", "february", "march", "april", 
                        "may", "june", "july", "august", 
                        "september", "october", "november", "december"};

// saves contents of sort without aguemnts to a specified file
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

// read the contents of the file, read character by character
// and split into strings
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

// loop through the truncated strings, check if
// the string is a month. If its a month, mark it on the bool array
void eval_months(int num_of_lines)
{
    int i, j, copy;

    for(i=0; i<=num_of_lines; i++)
    {
        copy=0;
        memset(temp_str, 0, STRING_SIZE);
        
        for(j=0; j<STRING_SIZE; j++)
        {
            if(eval[i][j] == '\n')
                temp_str[copy] = '\0';
            else    
                temp_str[copy] = eval[i][j];
            copy++;
        }

        for(j=0; j<12; j++)
        {
            if(strcmp(months[j], temp_str) == 0)
            {
                eval_bool[i] = j+3;
                break;
            }
        }
    }
}

// only evaluate non spaces
void truncate_b(int num_of_lines)
{
    int i, z, copy_idx;
    int eidx = 0;
    // only copy characters that are no spaces
    for(i=0; i<=num_of_lines; i++)
    {
        copy_idx = 0;
        memset(temp_str, 0, STRING_SIZE);
        for(z=0; z<STRING_SIZE;z++) 
        {
            if(eval[i][z] != ' ')
            {
                temp_str[copy_idx] = eval[i][z];
                copy_idx++;
            }
        }
        for(z=0; z<STRING_SIZE; z++) eval[i][z] = temp_str[z];
        eidx++; // increase index for next string
    }
}

// skip the spaces at the beggining and find characters used in sort
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
            if (file_contents[i][z] >= 33 && file_contents[i][z] <= 47) break;  // extra chars
            if (file_contents[i][z] >= 58 && file_contents[i][z] <= 64) break;  // extra chars
            if (file_contents[i][z] >= 123 && file_contents[i][z] <= 126) break;// extra chars
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
                    temp_str[0] += 32;
                else if(temp_str[0] >= 'a' && temp_str[0] <= 'z')  
                    eval_bool[i] = 1;
                else if(temp_str[0] >= '0' && temp_str[0] <= '9')
                    eval_bool[i] = 2;
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

// https://www.geeksforgeeks.org/selection-sort/
// selection sort based on ^
void sort(int n)
{
    int i, j, k, min_idx;
    
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n; i++)
    {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i+1; j <= n; j++)
        {
            int compare = strcmp(eval[j], eval[min_idx]);
            int flag = 0;
            if(compare == 0)
            {
                if(eval_bool[j] == 1 && eval_bool[min_idx] != 1)
                    flag=1;
            } 
            if (compare<0 || flag)
                min_idx = j;
        }
        // Swap the found minimum element with the first element
        if(min_idx != i)
        {
            for(k=0;k<STRING_SIZE;k++) temp_str[k] = eval[min_idx][k];
            for(k=0;k<STRING_SIZE;k++) eval[min_idx][k] = eval[i][k];
            for(k=0;k<STRING_SIZE;k++) eval[i][k] = temp_str[k];
            
            for(k=0;k<STRING_SIZE;k++) temp_str[k] = file_contents[min_idx][k];
            for(k=0;k<STRING_SIZE;k++) file_contents[min_idx][k] = file_contents[i][k];
            for(k=0;k<STRING_SIZE;k++) file_contents[i][k] = temp_str[k];
        }
            
    }
}

// find the strings that are numbers, mark them on the bool array, then sort based on the numbers.
// there is also a specific way to print them.
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

// after parsing the months, sort them based on the value on the bool matrix
void sort_months(int n)
{
    int i, j, k, flag;
    for(i=0; i<=n; i++)
    {                 
        for(j=i+1; j<=n; j++)
        {
            flag = 0;
            if(eval_bool[i] <= 2 && eval_bool[j] > 2)
            {
                flag = 1;
            }
            else if(eval_bool[i] > 2 && eval_bool[j] > 2)
            {
                if(eval_bool[i]>eval_bool[j]) flag=1;
            }

            if(flag)
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
    if(argc == 2)   // default
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
            printf(1, "%s", file_contents[i]);

    }
    else if(argc == 3)
    {
        if((fd = open(argv[2], 0)) < 0)
        {
            printf(1, "sort: cannot open file %s\n", argv[2]);
            exit();
        }
        int num_of_lines = read_contents(fd);
        
        if(strcmp(argv[1], "-b") == 0)
        {
            truncate_strings(num_of_lines);
            truncate_b(num_of_lines);
            sort(num_of_lines);
            for (int i = 0; i <= num_of_lines; i++)
                printf(1, "%s", file_contents[i]);
        }
        else if(strcmp(argv[1], "-r") == 0)
        {
            truncate_strings(num_of_lines);
            sort(num_of_lines);
            for (int i = num_of_lines; i >=0; i--)
                printf(1, "%s", file_contents[i]);    
        }
        else if(strcmp(argv[1], "-n") == 0)
        {
            sort_n(num_of_lines);
        }
        else if(strcmp(argv[1], "-M") == 0)
        {
            truncate_strings(num_of_lines);
            eval_months(num_of_lines);
            sort(num_of_lines);
            sort_months(num_of_lines);
            for (int i = 0; i <= num_of_lines; i++)
                printf(1, "%s", file_contents[i]);
        }
        else if(strcmp(argv[1], "-u") == 0)
        {   // basically sort, then uniq
            truncate_strings(num_of_lines);
            sort(num_of_lines);

            int index;
            char* cur;
            for(index=0; file_contents[index][0]!='\0';)
            {
                cur = file_contents[index];
                index++;
                // print as soon as you find a different string
                while(strcmp(file_contents[index], cur) == 0) index++;
                printf(1, "%s", cur);
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
