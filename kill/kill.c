#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/sysproto.h>

struct mykill_args {
    int pid;
    int signo;
    char *desc;
};

static int mykill(struct thread *td, void *args)
{
    struct mykill_args *uap = args;
    uprintf("mykill called. pid=%d, signo=%d, desc=%s\n", uap->pid, uap->signo, uap->desc);

    return 0;
}


static struct sysent mykill_sysent = {
    2,      /* number of arguments */
    mykill  /* function handling system call */
};


/* choose "the next" value later. */
static int offset = NO_SYSCALL;


static int load(struct module *module, int cmd, void *arg)
{
    int error = 0;

    switch (cmd) {
        case MOD_LOAD:
            uprintf("Loading module. Installing syscall at offset %d\n", offset);
            break;
        case MOD_UNLOAD:
            uprintf("Unloading module. syscall uninstalled from offset %d\n", offset);
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }

    return error;
}

//SYSCALL_MODULE(name, int *offset, struct sysent *new_sysent,
//  modeventhand_t evt, void *arg);
SYSCALL_MODULE(mykill, &offset, &mykill_sysent, load, NULL);
