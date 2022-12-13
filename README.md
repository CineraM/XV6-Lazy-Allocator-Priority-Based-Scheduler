# XV6 - Lazy Allocator-Priority Based Scheduler - Unix Sorting 
  
### Lazy Allocator 
Do not call growporc() while calling the sbrk system call. Implement growproc and allocvum whenever a trap state is started.  
  
### Priority Based Scheduler  
Add a priority variable in the proc struct. Assign a size-based priority to all processes. The scheduler picks the process with the highest priority.

### Unix Sort  
Implemented the Unix sorting alrorithm with bubble/selection/quick sort.  
  The following are supported.  
  * -b  
  * -r  
  * -n  
  * -M  
  * -u  
