## Overall design

The goal is to allow userspace programs access to the SMI interface and 
let them send and receive data over the bus. There can be only one userspace
program with access to the device at any one time. EEXIST will be returned
on open, if another application has already "taken" the device

app:
	
	- calls ioctl to set buffer size if default is not desired
	- calls ioctl to create DMA buffers (N) for input if > default N
	- foreach buffer, calls open() to get fd as either input or output
	- adds read-fd to epoll instance, 
	- create thread
		- calls epoll to wait on read
		- reads content of identified buffers
	- create thread
		- wait on some notification
		- write data to write-fd
		- expect it to go onto bus


driver:
	- implements:
		- poll  : 1 fd per buffer, when buffer has content, set bit
		- read	: accept fd and transfer data
		- write	: accept fd, transfer data, schedule bus traffic
		- ioctl : configure


Writes can be scheduled 'just-after-read', or 'immediate' (in which case
the application needs to prevent bus conflicts)

Content of the data is app-specific. Driver treats it as a blob of binary.
