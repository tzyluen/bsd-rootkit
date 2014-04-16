#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/syscall.h>
#include <sys/sysproto.h>

static int read_hook(struct thread *td, void *syscall_args)
{
    struct read_args /* {
        int fd;
        void *buf;
        size_t nbyte;
    } */ *uap;
    uap = (struct read_args *)syscall_args;

    int error;
    char buf[1];
    size_t done;

    /* read */
    error = sys_read(td, syscall_args);

    if (error || (!uap->nbyte) || (uap->nbyte > 1) || (uap->fd != 0))
        return error;
    copyinstr(uap->buf, buf, 1, &done);
    printf("%c\n", buf[0]);

    return error;
}

static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
        case MOD_LOAD:
            /* replace read with read_hook. */
            sysent[SYS_read].sy_call = (sy_call_t *)read_hook;
            break;
        case MOD_UNLOAD:
            /* change everything back to normal */
            sysent[SYS_read].sy_call = (sy_call_t *)sys_read;
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }
    return error;
}


static moduledata_t read_hook_mod = {
    "read_hook",        /* module name */
    load,               /* event handler */
    NULL                /* extra data */
};


DECLARE_MODULE(read_hook, read_hook_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
