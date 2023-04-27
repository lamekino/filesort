/* modified from: https://stackoverflow.com/a/2180157 */
/* posix c does not have a copy_file() SMH */
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <copyfile.h>
#else
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <linux/version.h>
#endif

int copy_file(const char* src, const char* dest)
{
    int input, output;
    if ((input = open(src, O_RDONLY)) == -1) {
        return -1;
    }
    if ((output = creat(dest, 0660)) == -1) {
        close(input);
        return -1;
    }

#if defined(__APPLE__) || defined(__FreeBSD__)
    /*
     * fcopyfile works on FreeBSD and OS X 10.5+
     */
    int result = fcopyfile(input, output, 0, COPYFILE_ALL);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
    /*
     * sendfile will work with non-socket output (i.e. regular file) on
     * Linux 2.6.33+
     */
    off_t bytesCopied = 0;
    struct stat fileinfo = {0};
    fstat(input, &fileinfo);
    int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
#else
    errno = ENOSUP;
    return -1;
#endif

    close(input);
    close(output);

    return result;
}

