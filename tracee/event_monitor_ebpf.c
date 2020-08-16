// +build ignore
// ^^ this is a golang build tag meant to exclude this C file from compilation by the CGO compiler

#include <uapi/linux/ptrace.h>
#include <uapi/linux/in.h>
#include <uapi/linux/in6.h>
#include <uapi/linux/un.h>
#include <uapi/linux/utsname.h>
#include <linux/binfmts.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm_types.h>
#include <linux/mount.h>
#include <linux/nsproxy.h>
#include <linux/ns_common.h>
#include <linux/pid_namespace.h>
#include <linux/security.h>
#include <linux/socket.h>
#include <linux/version.h>

#define MAX_PERCPU_BUFSIZE  (1 << 15)     // This value is actually set by the kernel as an upper bound
#define MAX_STRING_SIZE     4096          // Choosing this value to be the same as PATH_MAX
#define MAX_STR_ARR_ELEM    40            // String array elements number should be bounded due to instructions limit
#define MAX_PATH_PREF_SIZE  64            // Max path prefix should be bounded due to instructions limit

#define SUBMIT_BUF_IDX      0
#define STRING_BUF_IDX      1
#define FILE_BUF_IDX        2
#define MAX_BUFFERS         3

#define SEND_VFS_WRITE      1
#define SEND_MPROTECT       2
#define SEND_META_SIZE      20

#define ALERT_MMAP_W_X      1
#define ALERT_MPROT_X_ADD   2
#define ALERT_MPROT_W_ADD   3
#define ALERT_MPROT_W_REM   4

#define TAIL_VFS_WRITE      0
#define TAIL_SEND_BIN       1
#define MAX_TAIL_CALL       2

#define NONE_T        0UL
#define INT_T         1UL
#define UINT_T        2UL
#define LONG_T        3UL
#define ULONG_T       4UL
#define OFF_T_T       5UL
#define MODE_T_T      6UL
#define DEV_T_T       7UL
#define SIZE_T_T      8UL
#define POINTER_T     9UL
#define STR_T         10UL
#define STR_ARR_T     11UL
#define SOCKADDR_T    12UL
#define ALERT_T       13UL
#define TYPE_MAX      255UL

#define TAG_NONE           0UL
#define TAG_FD             1UL
#define TAG_FILENAME       2UL
#define TAG_PATHNAME       3UL
#define TAG_ARGV           4UL
#define TAG_ENVP           5UL
#define TAG_DEV            6UL
#define TAG_INODE          7UL
#define TAG_DIRFD          8UL
#define TAG_FLAGS          9UL
#define TAG_CAP            10UL
#define TAG_SYSCALL        11UL
#define TAG_COUNT          12UL
#define TAG_POS            13UL
#define TAG_ALERT          14UL
#define TAG_MODE           15UL
#define TAG_ADDR           16UL
#define TAG_LENGTH         17UL
#define TAG_PROT           18UL
#define TAG_OFFSET         19UL
#define TAG_PKEY           20UL
#define TAG_NAME           21UL
#define TAG_OLDFD          22UL
#define TAG_NEWFD          23UL
#define TAG_DOMAIN         24UL
#define TAG_TYPE           25UL
#define TAG_PROTOCOL       26UL
#define TAG_REQUEST        27UL
#define TAG_PID            28UL
#define TAG_SIG            29UL
#define TAG_SOCKFD         30UL
#define TAG_BACKLOG        31UL
#define TAG_OPTION         32UL
#define TAG_ARG2           33UL
#define TAG_ARG3           34UL
#define TAG_ARG4           35UL
#define TAG_ARG5           36UL
#define TAG_DATA           37UL
#define TAG_LOCAL_IOV      38UL
#define TAG_LIOVCNT        39UL
#define TAG_REMOTE_IOV     40UL
#define TAG_RIOVCNT        41UL
#define TAG_MODULE_IMAGE   42UL
#define TAG_LEN            43UL
#define TAG_PARAM_VALUES   44UL
#define TAG_TARGET         45UL
#define TAG_NEWDIRFD       46UL
#define TAG_LINKPATH       47UL
#define TAG_SOURCE         48UL
#define TAG_FILESYSTEMTYPE 49UL
#define TAG_MOUNTFLAGS     50UL
#define TAG_UID            51UL
#define TAG_GID            52UL
#define TAG_FSUID          53UL
#define TAG_FSGID          54UL
#define TAG_RUID           55UL
#define TAG_EUID           56UL
#define TAG_RGID           57UL
#define TAG_EGID           58UL
#define TAG_SUID           59UL
#define TAG_SGID           60UL
#define TAG_OWNER          61UL
#define TAG_GROUP          62UL
#define TAG_BUF            63UL
#define TAG_WHENCE         64UL
#define TAG_ADVICE         65UL
#define TAG_DEST_ADDR      66UL
#define TAG_SRC_ADDR       67UL

#define CONFIG_MODE             0
#define CONFIG_SHOW_SYSCALL     1
#define CONFIG_EXEC_ENV         2
#define CONFIG_CAPTURE_FILES    3
#define CONFIG_EXTRACT_DYN_CODE 4

#define MODE_SYSTEM     0
#define MODE_PID        1
#define MODE_CONTAINER  2

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
#error Minimal required kernel version is 4.14
#endif

/*==================================== ENUMS =================================*/

enum event_id {
    SYS_READ,
    SYS_WRITE,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_STAT,
    SYS_FSTAT,
    SYS_LSTAT,
    SYS_POLL,
    SYS_LSEEK,
    SYS_MMAP,
    SYS_MPROTECT,
    SYS_MUNMAP,
    SYS_BRK,
    SYS_RT_SIGACTION,
    SYS_RT_SIGPROCMASK,
    SYS_RT_SIGRETURN,
    SYS_IOCTL,
    SYS_PREAD64,
    SYS_PWRITE64,
    SYS_READV,
    SYS_WRITEV,
    SYS_ACCESS,
    SYS_PIPE,
    SYS_SELECT,
    SYS_SCHED_YIELD,
    SYS_MREMAP,
    SYS_MSYNC,
    SYS_MINCORE,
    SYS_MADVISE,
    SYS_SHMGET,
    SYS_SHMAT,
    SYS_SHMCTL,
    SYS_DUP,
    SYS_DUP2,
    SYS_PAUSE,
    SYS_NANOSLEEP,
    SYS_GETITIMER,
    SYS_ALARM,
    SYS_SETITIMER,
    SYS_GETPID,
    SYS_SENDFILE,
    SYS_SOCKET,
    SYS_CONNECT,
    SYS_ACCEPT,
    SYS_SENDTO,
    SYS_RECVFROM,
    SYS_SENDMSG,
    SYS_RECVMSG,
    SYS_SHUTDOWN,
    SYS_BIND,
    SYS_LISTEN,
    SYS_GETSOCKNAME,
    SYS_GETPEERNAME,
    SYS_SOCKETPAIR,
    SYS_SETSOCKOPT,
    SYS_GETSOCKOPT,
    SYS_CLONE,
    SYS_FORK,
    SYS_VFORK,
    SYS_EXECVE,
    SYS_EXIT,
    SYS_WAIT4,
    SYS_KILL,
    SYS_UNAME,
    SYS_SEMGET,
    SYS_SEMOP,
    SYS_SEMCTL,
    SYS_SHMDT,
    SYS_MSGGET,
    SYS_MSGSND,
    SYS_MSGRCV,
    SYS_MSGCTL,
    SYS_FCNTL,
    SYS_FLOCK,
    SYS_FSYNC,
    SYS_FDATASYNC,
    SYS_TRUNCATE,
    SYS_FTRUNCATE,
    SYS_GETDENTS,
    SYS_GETCWD,
    SYS_CHDIR,
    SYS_FCHDIR,
    SYS_RENAME,
    SYS_MKDIR,
    SYS_RMDIR,
    SYS_CREAT,
    SYS_LINK,
    SYS_UNLINK,
    SYS_SYMLINK,
    SYS_READLINK,
    SYS_CHMOD,
    SYS_FCHMOD,
    SYS_CHOWN,
    SYS_FCHOWN,
    SYS_LCHOWN,
    SYS_UMASK,
    SYS_GETTIMEOFDAY,
    SYS_GETRLIMIT,
    SYS_GETRUSAGE,
    SYS_SYSINFO,
    SYS_TIMES,
    SYS_PTRACE,
    SYS_GETUID,
    SYS_SYSLOG,
    SYS_GETGID,
    SYS_SETUID,
    SYS_SETGID,
    SYS_GETEUID,
    SYS_GETEGID,
    SYS_SETPGID,
    SYS_GETPPID,
    SYS_GETPGRP,
    SYS_SETSID,
    SYS_SETREUID,
    SYS_SETREGID,
    SYS_GETGROUPS,
    SYS_SETGROUPS,
    SYS_SETRESUID,
    SYS_GETRESUID,
    SYS_SETRESGID,
    SYS_GETRESGID,
    SYS_GETPGID,
    SYS_SETFSUID,
    SYS_SETFSGID,
    SYS_GETSID,
    SYS_CAPGET,
    SYS_CAPSET,
    SYS_RT_SIGPENDING,
    SYS_RT_SIGTIMEDWAIT,
    SYS_RT_SIGQUEUEINFO,
    SYS_RT_SIGSUSPEND,
    SYS_SIGALTSTACK,
    SYS_UTIME,
    SYS_MKNOD,
    SYS_USELIB,
    SYS_PERSONALITY,
    SYS_USTAT,
    SYS_STATFS,
    SYS_FSTATFS,
    SYS_SYSFS,
    SYS_GETPRIORITY,
    SYS_SETPRIORITY,
    SYS_SCHED_SETPARAM,
    SYS_SCHED_GETPARAM,
    SYS_SCHED_SETSCHEDULER,
    SYS_SCHED_GETSCHEDULER,
    SYS_SCHED_GET_PRIORITY_MAX,
    SYS_SCHED_GET_PRIORITY_MIN,
    SYS_SCHED_RR_GET_INTERVAL,
    SYS_MLOCK,
    SYS_MUNLOCK,
    SYS_MLOCKALL,
    SYS_MUNLOCKALL,
    SYS_VHANGUP,
    SYS_MODIFY_LDT,
    SYS_PIVOT_ROOT,
    SYS_SYSCTL,
    SYS_PRCTL,
    SYS_ARCH_PRCTL,
    SYS_ADJTIMEX,
    SYS_SETRLIMIT,
    SYS_CHROOT,
    SYS_SYNC,
    SYS_ACCT,
    SYS_SETTIMEOFDAY,
    SYS_MOUNT,
    SYS_UMOUNT,
    SYS_SWAPON,
    SYS_SWAPOFF,
    SYS_REBOOT,
    SYS_SETHOSTNAME,
    SYS_SETDOMAINNAME,
    SYS_IOPL,
    SYS_IOPERM,
    SYS_CREATE_MODULE,
    SYS_INIT_MODULE,
    SYS_DELETE_MODULE,
    SYS_GET_KERNEL_SYMS,
    SYS_QUERY_MODULE,
    SYS_QUOTACTL,
    SYS_NFSSERVCTL,
    SYS_GETPMSG,
    SYS_PUTPMSG,
    SYS_AFS,
    SYS_TUXCALL,
    SYS_SECURITY,
    SYS_GETTID,
    SYS_READAHEAD,
    SYS_SETXATTR,
    SYS_LSETXATTR,
    SYS_FSETXATTR,
    SYS_GETXATTR,
    SYS_LGETXATTR,
    SYS_FGETXATTR,
    SYS_LISTXATTR,
    SYS_LLISTXATTR,
    SYS_FLISTXATTR,
    SYS_REMOVEXATTR,
    SYS_LREMOVEXATTR,
    SYS_FREMOVEXATTR,
    SYS_TKILL,
    SYS_TIME,
    SYS_FUTEX,
    SYS_SCHED_SETAFFINITY,
    SYS_SCHED_GETAFFINITY,
    SYS_SET_THREAD_AREA,
    SYS_IO_SETUP,
    SYS_IO_DESTROY,
    SYS_IO_GETEVENTS,
    SYS_IO_SUBMIT,
    SYS_IO_CANCEL,
    SYS_GET_THREAD_AREA,
    SYS_LOOOKUP_DCOOKIE,
    SYS_EPOLL_CREATE,
    SYS_EPOLL_CTL_OLD,
    SYS_EPOLL_WAIT_OLD,
    SYS_REMAP_FILE_PAGES,
    SYS_GETDENTS64,
    SYS_SET_TID_ADDRESS,
    SYS_RESTART_SYSCALL,
    SYS_SEMTIMEDOP,
    SYS_FADVISE64,
    SYS_TIMER_CREATE,
    SYS_TIMER_SETTIME,
    SYS_TIMER_GETTIME,
    SYS_TIMER_GETOVERRUN,
    SYS_TIMER_DELETE,
    SYS_CLOCK_SETTIME,
    SYS_CLOCK_GETTIME,
    SYS_CLOCK_GETRES,
    SYS_CLOCK_NANOSLEEP,
    SYS_EXIT_GROUP,
    SYS_EPOLL_WAIT,
    SYS_EPOLL_CTL,
    SYS_TGKILL,
    SYS_UTIMES,
    SYS_VSERVER,
    SYS_MBIND,
    SYS_SET_MEMPOLICY,
    SYS_GET_MEMPOLICY,
    SYS_MQ_OPEN,
    SYS_MQ_UNLINK,
    SYS_MQ_TIMEDSEND,
    SYS_MQ_TIMEDRECEIVE,
    SYS_MQ_NOTIFY,
    SYS_MQ_GETSETATTR,
    SYS_KEXEC_LOAD,
    SYS_WAITID,
    SYS_ADD_KEY,
    SYS_REQUEST_KEY,
    SYS_KEYCTL,
    SYS_IOPRIO_SET,
    SYS_IOPRIO_GET,
    SYS_INOTIFY_INIT,
    SYS_INOTIFY_ADD_WATCH,
    SYS_INOTIFY_RM_WATCH,
    SYS_MIGRATE_PAGES,
    SYS_OPENAT,
    SYS_MKDIRAT,
    SYS_MKNODAT,
    SYS_FCHOWNAT,
    SYS_FUTIMESAT,
    SYS_NEWFSTATAT,
    SYS_UNLINKAT,
    SYS_RENAMEAT,
    SYS_LINKAT,
    SYS_SYMLINKAT,
    SYS_READLINKAT,
    SYS_FCHMODAT,
    SYS_FACCESSAT,
    SYS_PSELECT6,
    SYS_PPOLL,
    SYS_UNSHARE,
    SYS_SET_ROBUST_LIST,
    SYS_GET_ROBUST_LIST,
    SYS_SPLICE,
    SYS_TEE,
    SYS_SYNC_FILE_RANGE,
    SYS_VMSPLICE,
    SYS_MOVE_PAGES,
    SYS_UTIMENSAT,
    SYS_EPOLL_PWAIT,
    SYS_SIGNALFD,
    SYS_TIMERFD_CREATE,
    SYS_EVENTFD,
    SYS_FALLOCATE,
    SYS_TIMERFD_SETTIME,
    SYS_TIMERFD_GETTIME,
    SYS_ACCEPT4,
    SYS_SIGNALFD4,
    SYS_EVENTFD2,
    SYS_EPOLL_CREATE1,
    SYS_DUP3,
    SYS_PIPE2,
    SYS_INOTIFY_INIT1,
    SYS_PREADV,
    SYS_PWRITEV,
    SYS_RT_TGSIGQUEUEINFO,
    SYS_PERF_EVENT_OPEN,
    SYS_RECVMMSG,
    SYS_FANOTIFY_INIT,
    SYS_FANOTIFY_MARK,
    SYS_PRLIMIT64,
    SYS_NAME_TO_HANDLE_AT,
    SYS_OPEN_BY_HANDLE_AT,
    SYS_CLOCK_ADJTIME,
    SYS_SYNCFS,
    SYS_SENDMMSG,
    SYS_SETNS,
    SYS_GETCPU,
    SYS_PROCESS_VM_READV,
    SYS_PROCESS_VM_WRITEV,
    SYS_KCMP,
    SYS_FINIT_MODULE,
    SYS_SCHED_SETATTR,
    SYS_SCHED_GETATTR,
    SYS_RENAMEAT2,
    SYS_SECCOMPP,
    SYS_GETRANDOM,
    SYS_MEMFD_CREATE,
    SYS_KEXEC_FILE_LOAD,
    SYS_BPF,
    SYS_EXECVEAT,
    SYS_USERFAULTFD,
    SYS_MEMBARRIER,
    SYS_MLOCK2,
    SYS_COPY_FILE_RANGE,
    SYS_PREADV2,
    SYS_PWRITEV2,
    SYS_PKEY_MPROTECT,
    SYS_PKEY_ALLOC,
    SYS_PKRY_FREE,
    SYS_STATX,
    SYS_IO_PGETEVENTS,
    SYS_RSEQ,
    RESERVED335,
    RESERVED336,
    RESERVED337,
    RESERVED338,
    RESERVED339,
    RESERVED340,
    RESERVED341,
    RESERVED342,
    RESERVED343,
    RESERVED344,
    RESERVED345,
    RESERVED346,
    RESERVED347,
    RESERVED348,
    RESERVED349,
    RAW_SYSCALLS,
    DO_EXIT,
    CAP_CAPABLE,
    SECURITY_BPRM_CHECK,
    SECURITY_FILE_OPEN,
    VFS_WRITE,
    MEM_PROT_ALERT,
};

/*=============================== INTERNAL STRUCTS ===========================*/

typedef struct context {
    u64 ts;                     // Timestamp
    u32 pid;                    // PID as in the userspace term
    u32 tid;                    // TID as in the userspace term
    u32 ppid;                   // Parent PID as in the userspace term
    u32 host_pid;               // PID in host pid namespace
    u32 host_tid;               // TID in host pid namespace
    u32 host_ppid;              // Parent PID in host pid namespace
    u32 uid;
    u32 mnt_id;
    u32 pid_id;
    char comm[TASK_COMM_LEN];
    char uts_name[TASK_COMM_LEN];
    enum event_id eventid;
    s64 retval;
    u8 argnum;
} context_t;

typedef struct args {
    unsigned long args[6];
} args_t;

typedef struct bin_args {
    u8 type;
    u8 metadata[SEND_META_SIZE];
    char *ptr;
    loff_t start_off;
    unsigned int full_size;
} bin_args_t;

typedef struct simple_buf {
    u8 buf[MAX_PERCPU_BUFSIZE];
} buf_t;

typedef struct path_filter {
    char path[MAX_PATH_PREF_SIZE];
} path_filter_t;

typedef struct alert {
    u64 ts;     // Timestamp
    u32 msg;    // Encoded message
    u8 payload; // Non zero if payload is sent to userspace
} alert_t;

/*================================ KERNEL STRUCTS =============================*/

struct mnt_namespace {
    atomic_t        count;
    struct ns_common    ns;
    // ...
};

struct uts_namespace {
    struct kref kref;
    struct new_utsname name;
    // ...
};

struct mount {
    struct hlist_node mnt_hash;
    struct mount *mnt_parent;
    struct dentry *mnt_mountpoint;
    struct vfsmount mnt;
    // ...
};

/*=================================== MAPS =====================================*/

BPF_HASH(config_map, u32, u32);                     // Various configurations
BPF_HASH(chosen_events_map, u32, u32);              // Various configurations
BPF_HASH(pids_map, u32, u32);                       // Save container pid namespaces
BPF_HASH(args_map, u64, args_t);                    // Persist args info between function entry and return
BPF_HASH(bin_args_map, u64, bin_args_t);            // Persist args for send_bin funtion
BPF_ARRAY(file_filter, path_filter_t, 3);           // Used to filter vfs_write events
BPF_PERCPU_ARRAY(bufs, buf_t, MAX_BUFFERS);         // Percpu global buffer variables
BPF_PERCPU_ARRAY(bufs_off, u32, MAX_BUFFERS);       // Holds offsets to bufs respectively
BPF_PROG_ARRAY(prog_array, MAX_TAIL_CALL);          // Used to store programs for tail calls

/*================================== EVENTS ====================================*/

BPF_PERF_OUTPUT(events);                            // Events submission
BPF_PERF_OUTPUT(file_writes);                       // File writes events submission

/*================== KERNEL VERSION DEPENDANT HELPER FUNCTIONS =================*/

static __always_inline u32 get_task_mnt_ns_id(struct task_struct *task)
{
    return task->nsproxy->mnt_ns->ns.inum;
}

static __always_inline u32 get_task_pid_ns_id(struct task_struct *task)
{
    return task->nsproxy->pid_ns_for_children->ns.inum;
}

static __always_inline u32 get_task_ns_pid(struct task_struct *task)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    // kernel 4.14-4.18:
    return task->pids[PIDTYPE_PID].pid->numbers[task->nsproxy->pid_ns_for_children->level].nr;
#else
    // kernel 4.19 onwards:
    return task->thread_pid->numbers[task->nsproxy->pid_ns_for_children->level].nr;
#endif
}

static __always_inline u32 get_task_ns_tgid(struct task_struct *task)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    // kernel 4.14-4.18:
    return task->group_leader->pids[PIDTYPE_PID].pid->numbers[task->nsproxy->pid_ns_for_children->level].nr;
#else
    // kernel 4.19 onwards:
    return task->group_leader->thread_pid->numbers[task->nsproxy->pid_ns_for_children->level].nr;
#endif
}

static __always_inline u32 get_task_ns_ppid(struct task_struct *task)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    // kernel 4.14-4.18:
    return task->real_parent->pids[PIDTYPE_PID].pid->numbers[task->real_parent->nsproxy->pid_ns_for_children->level].nr;
#else
    // kernel 4.19 onwards:
    return task->real_parent->thread_pid->numbers[task->real_parent->nsproxy->pid_ns_for_children->level].nr;
#endif
}

static __always_inline char * get_task_uts_name(struct task_struct *task)
{
    return task->nsproxy->uts_ns->name.nodename;
}

static __always_inline u32 get_task_ppid(struct task_struct *task)
{
    return task->real_parent->pid;
}

static __always_inline void get_syscall_args(struct pt_regs *ctx, args_t *args)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)
    args->args[0] = PT_REGS_PARM1(ctx);
    args->args[1] = PT_REGS_PARM2(ctx);
    args->args[2] = PT_REGS_PARM3(ctx);
    args->args[3] = PT_REGS_PARM4(ctx);
    args->args[4] = PT_REGS_PARM5(ctx);
    args->args[5] = PT_REGS_PARM6(ctx);
#else
    struct pt_regs * ctx2 = (struct pt_regs *)ctx->di;
    bpf_probe_read(&args->args[0], sizeof(args->args[0]), &ctx2->di);
    bpf_probe_read(&args->args[1], sizeof(args->args[1]), &ctx2->si);
    bpf_probe_read(&args->args[2], sizeof(args->args[2]), &ctx2->dx);
    bpf_probe_read(&args->args[3], sizeof(args->args[3]), &ctx2->r10);
    bpf_probe_read(&args->args[4], sizeof(args->args[4]), &ctx2->r8);
    bpf_probe_read(&args->args[5], sizeof(args->args[5]), &ctx2->r9);
#endif
}

static __always_inline struct pt_regs* get_task_pt_regs()
{
    struct task_struct *task = (struct task_struct *)bpf_get_current_task();
    void* task_stack_page = task->stack;
    void* __ptr = task_stack_page + THREAD_SIZE - TOP_OF_KERNEL_STACK_PADDING;
    return ((struct pt_regs *)__ptr) - 1;
}

/*============================== HELPER FUNCTIONS ==============================*/

// re-define container_of as bcc complains
#define my_container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })

static inline struct mount *real_mount(struct vfsmount *mnt)
{
    return my_container_of(mnt, struct mount, mnt);
}

static __inline int is_prefix(char *prefix, char *str)
{
    int i;
    #pragma unroll
    for (i = 0; i < MAX_PATH_PREF_SIZE; prefix++, str++, i++) {
        if (!*prefix)
            return 1;
        if (*prefix != *str) {
            return 0;
        }
    }

    // prefix is too long
    return 0;
}

static __always_inline u32 lookup_pid()
{
    u32 pid = bpf_get_current_pid_tgid();
    if (pids_map.lookup(&pid) == 0)
        return 0;

    return pid;
}

static __always_inline u32 lookup_pid_ns(struct task_struct *task)
{
    u32 task_pid_ns = get_task_pid_ns_id(task);

    u32 *pid_ns = pids_map.lookup(&task_pid_ns);
    if (pid_ns == 0)
        return 0;

    return *pid_ns;
}

static __always_inline void add_pid_fork(u32 pid)
{
    pids_map.update(&pid, &pid);
}

static __always_inline u32 add_pid()
{
    u32 pid = bpf_get_current_pid_tgid();
    if (pids_map.lookup(&pid) == 0)
        pids_map.update(&pid, &pid);

    return pid;
}

static __always_inline u32 add_pid_ns_if_needed()
{
    struct task_struct *task;
    task = (struct task_struct *)bpf_get_current_task();

    u32 pid_ns = get_task_pid_ns_id(task);
    if (pids_map.lookup(&pid_ns) != 0)
        // Container pidns was already added to map
        return pid_ns;

    // If pid equals 1 - start tracing the container
    if (get_task_ns_pid(task) == 1) {
        // A new container/pod was started - add pid namespace to map
        pids_map.update(&pid_ns, &pid_ns);
        return pid_ns;
    }

    // Not a container/pod
    return 0;
}

static __always_inline void remove_pid()
{
    u32 pid = bpf_get_current_pid_tgid();
    if (pids_map.lookup(&pid) != 0)
        pids_map.delete(&pid);
}

static __always_inline void remove_pid_ns_if_needed()
{
    struct task_struct *task;
    task = (struct task_struct *)bpf_get_current_task();

    u32 pid_ns = get_task_pid_ns_id(task);
    if (pids_map.lookup(&pid_ns) != 0) {
        // If pid equals 1 - stop tracing this pid namespace
        if (get_task_ns_pid(task) == 1) {
            pids_map.delete(&pid_ns);
        }
    }
}

static __always_inline int get_config(u32 key)
{
    u32 *config = config_map.lookup(&key);

    if (config == NULL)
        return 0;

    return *config;
}

static __always_inline int should_trace()
{
    struct task_struct *task = (struct task_struct *)bpf_get_current_task();

    u32 rc = 0;
    if (get_config(CONFIG_MODE) == MODE_CONTAINER)
        rc = lookup_pid_ns(task);
    else
        rc = lookup_pid();

    return rc;
}

static __always_inline int event_chosen(u32 key)
{
    u32 *config = chosen_events_map.lookup(&key);
    if (config == NULL)
        return 0;

    return *config;
}

static __always_inline int init_context(context_t *context)
{
    struct task_struct *task;
    task = (struct task_struct *)bpf_get_current_task();

    u64 id = bpf_get_current_pid_tgid();
    context->host_tid = id;
    context->host_pid = id >> 32;
    context->host_ppid = get_task_ppid(task);
    context->tid = get_task_ns_pid(task);
    context->pid = get_task_ns_tgid(task);
    context->ppid = get_task_ns_ppid(task);
    context->mnt_id = get_task_mnt_ns_id(task);
    context->pid_id = get_task_pid_ns_id(task);
    context->uid = bpf_get_current_uid_gid();
    bpf_get_current_comm(&context->comm, sizeof(context->comm));
    char * uts_name = get_task_uts_name(task);
    if (uts_name)
        bpf_probe_read_str(&context->uts_name, TASK_COMM_LEN, uts_name);

    // Save timestamp in microsecond resolution
    context->ts = bpf_ktime_get_ns()/1000;

    return 0;
}

static __always_inline buf_t* get_buf(int idx)
{
    return bufs.lookup(&idx);
}

static __always_inline void set_buf_off(int buf_idx, u32 new_off)
{
    bufs_off.update(&buf_idx, &new_off);
}

static __always_inline u32* get_buf_off(int buf_idx)
{
    return bufs_off.lookup(&buf_idx);
}

// Context will always be at the start of the submission buffer
// It may be needed to resave the context if the arguments number changed by logic
static __always_inline int save_context_to_buf(buf_t *submit_p, void *ptr)
{
    int rc = bpf_probe_read(&(submit_p->buf[0]), sizeof(context_t), ptr);
    if (rc == 0)
        return sizeof(context_t);

    return 0;
}

static __always_inline int save_to_submit_buf(buf_t *submit_p, void *ptr, int size, u8 type, u8 tag)
{
// The biggest element that can be saved with this function should be defined here
#define MAX_ELEMENT_SIZE sizeof(struct sockaddr_un)

    if (type == 0)
        return 0;

    u32* off = get_buf_off(SUBMIT_BUF_IDX);
    if (off == NULL)
        return -1;
    if (*off > MAX_PERCPU_BUFSIZE - MAX_ELEMENT_SIZE)
        // Satisfy validator for probe read
        return 0;

    // Save argument type
    int rc = bpf_probe_read(&(submit_p->buf[*off]), 1, &type);
    if (rc != 0)
        return 0;

    *off += 1;

    // Save argument tag
    if (tag != TAG_NONE) {
        rc = bpf_probe_read(&(submit_p->buf[*off & (MAX_PERCPU_BUFSIZE-1)]), 1, &tag);
        if (rc != 0)
            return 0;

        *off += 1;
    }

    if (*off > MAX_PERCPU_BUFSIZE - MAX_ELEMENT_SIZE)
        // Satisfy validator for probe read
        return 0;

    // Read into buffer
    rc = bpf_probe_read(&(submit_p->buf[*off]), size, ptr);
    if (rc == 0) {
        *off += size;
        set_buf_off(SUBMIT_BUF_IDX, *off);
        return size;
    }

    return 0;
}

static __always_inline int save_str_to_buf(buf_t *submit_p, void *ptr, u8 tag)
{
    u32* off = get_buf_off(SUBMIT_BUF_IDX);
    if (off == NULL)
        return -1;
    if (*off > MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE - sizeof(int))
        // not enough space - return
        return 0;

    // Save argument type
    u8 type = STR_T;
    bpf_probe_read(&(submit_p->buf[*off & (MAX_PERCPU_BUFSIZE-1)]), 1, &type);

    *off += 1;

    // Save argument tag
    if (tag != TAG_NONE) {
        int rc = bpf_probe_read(&(submit_p->buf[*off & (MAX_PERCPU_BUFSIZE-1)]), 1, &tag);
        if (rc != 0)
            return 0;

        *off += 1;
    }

    if (*off > MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE - sizeof(int))
        // Satisfy validator for probe read
        return 0;

    // Read into buffer
    int sz = bpf_probe_read_str(&(submit_p->buf[*off + sizeof(int)]), MAX_STRING_SIZE, ptr);
    if (sz > 0) {
        if (*off > MAX_PERCPU_BUFSIZE - sizeof(int))
            // Satisfy validator for probe read
            return 0;
        bpf_probe_read(&(submit_p->buf[*off]), sizeof(int), &sz);
        *off += sz + sizeof(int);
        set_buf_off(SUBMIT_BUF_IDX, *off);
        return sz + sizeof(int);
    }

    return 0;
}

static __always_inline int save_str_arr_to_buf(buf_t *submit_p, const char __user *const __user *ptr, u8 tag)
{
    u8 elem_num = 0;

    // mark string array start
    save_to_submit_buf(submit_p, NULL, 0, STR_ARR_T, tag);

    u32* off = get_buf_off(SUBMIT_BUF_IDX);
    if (off == NULL)
        return -1;
    // Save space for number of elements
    u32 orig_off = *off;
    *off += 1;

    #pragma unroll
    for (int i = 0; i < MAX_STR_ARR_ELEM; i++) {
        const char *argp = NULL;
        bpf_probe_read(&argp, sizeof(argp), &ptr[i]);
        if (!argp)
            goto out;

        if (*off > MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE - sizeof(int))
            // not enough space - return
            goto out;

        // Read into buffer
        int sz = bpf_probe_read_str(&(submit_p->buf[*off + sizeof(int)]), MAX_STRING_SIZE, argp);
        if (sz > 0) {
            if (*off > MAX_PERCPU_BUFSIZE - sizeof(int))
                // Satisfy validator for probe read
                goto out;
            bpf_probe_read(&(submit_p->buf[*off]), sizeof(int), &sz);
            *off += sz + sizeof(int);
            elem_num++;
            continue;
        } else {
            goto out;
        }
    }
    // handle truncated argument list
    char ellipsis[] = "...";
    if (*off > MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE - sizeof(int))
        // not enough space - return
        goto out;

    // Read into buffer
    int sz = bpf_probe_read_str(&(submit_p->buf[*off + sizeof(int)]), MAX_STRING_SIZE, ellipsis);
    if (sz > 0) {
        if (*off > MAX_PERCPU_BUFSIZE - sizeof(int))
            // Satisfy validator for probe read
            goto out;
        bpf_probe_read(&(submit_p->buf[*off]), sizeof(int), &sz);
        *off += sz + sizeof(int);
        elem_num++;
    }
out:
    set_buf_off(SUBMIT_BUF_IDX, *off);
    // save number of elements in the array
    bpf_probe_read(&(submit_p->buf[orig_off & (MAX_PERCPU_BUFSIZE-1)]), 1, &elem_num);
    return 0;
}

static __always_inline int get_path_string(buf_t *string_p, struct path *path)
{
    char slash = '/';
    int zero = 0;
    struct dentry *dentry = path->dentry;;
    struct vfsmount *vfsmnt = path->mnt;
    struct mount mnt;
    struct mount *mnt_p = real_mount(vfsmnt);
    bpf_probe_read(&mnt, sizeof(struct mount), mnt_p);

    u32 buf_off = MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE;

    #pragma unroll
    // As bpf loops are not allowed and max instructions number is 4096, path components is limited to 30
    for (int i = 0; i < 30; i++) {
        if (dentry == vfsmnt->mnt_root || dentry == dentry->d_parent) {
            if (dentry != vfsmnt->mnt_root) {
                // We reached root, but not mount root - escaped?
                break;
            }
            if (mnt_p != mnt.mnt_parent) {
                // We reached root, but not global root - continue with mount point path
                dentry = mnt.mnt_mountpoint;
                bpf_probe_read(&mnt, sizeof(struct mount), mnt.mnt_parent);
                vfsmnt = &mnt.mnt;
                continue;
            }
            // Global root - path fully parsed
            break;
        }
        // Add this dentry name to path
        unsigned int len = (dentry->d_name.len+1) & (MAX_STRING_SIZE-1);
        unsigned int off = buf_off - len;
        // Is string buffer big enough for dentry name?
        int sz = 0;
        if (off <= MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE)
            sz = bpf_probe_read_str(&(string_p->buf[off]), len, (void *)dentry->d_name.name);
        else
            break;
        if (sz > 1) {
            buf_off -= 1; // remove null byte termination with slash sign
            bpf_probe_read(&(string_p->buf[buf_off & (MAX_PERCPU_BUFSIZE-1)]), 1, &slash);
            buf_off -= sz - 1;
        } else {
            // If sz is 0 or 1 we have an error (path can't be null nor an empty string)
            break;
        }
        dentry = dentry->d_parent;
    }

    if (buf_off == MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE) {
	// memfd files have no path in the filesystem -> extract their name
        buf_off = 0;
        int sz = bpf_probe_read_str(&(string_p->buf[0]), MAX_STRING_SIZE, (void *)dentry->d_name.name);
    } else {
        // Add leading slash
        buf_off -= 1;
        bpf_probe_read(&(string_p->buf[buf_off & (MAX_PERCPU_BUFSIZE-1)]), 1, &slash);
        // Null terminate the path string
        bpf_probe_read(&(string_p->buf[MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE-1]), 1, &zero);
    }

    set_buf_off(STRING_BUF_IDX, buf_off);
    return buf_off;
}

static __always_inline int events_perf_submit(struct pt_regs *ctx)
{
    u32* off = get_buf_off(SUBMIT_BUF_IDX);
    if (off == NULL)
        return -1;
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return -1;

    /* satisfy validator by setting buffer bounds */
    int size = *off & (MAX_PERCPU_BUFSIZE-1);
    void * data = submit_p->buf;
    return events.perf_submit(ctx, data, size);
}

static __always_inline int is_container()
{
    struct task_struct *task;
    task = (struct task_struct *)bpf_get_current_task();

    return lookup_pid_ns(task);
}

static __always_inline int save_args(struct pt_regs *ctx, u32 event_id, bool is_syscall)
{
    u64 id;
    u32 tid;
    args_t args = {};

    if (!is_syscall) {
        args.args[0] = PT_REGS_PARM1(ctx);
        args.args[1] = PT_REGS_PARM2(ctx);
        args.args[2] = PT_REGS_PARM3(ctx);
        args.args[3] = PT_REGS_PARM4(ctx);
        args.args[4] = PT_REGS_PARM5(ctx);
        args.args[5] = PT_REGS_PARM6(ctx);
    } else {
        get_syscall_args(ctx, &args);
    }

    id = event_id;
    tid = bpf_get_current_pid_tgid();
    id = id << 32 | tid;
    args_map.update(&id, &args);

    return 0;
}

static __always_inline int load_args(args_t *args, bool delete, u32 event_id)
{
    args_t *saved_args;
    u32 tid = bpf_get_current_pid_tgid();
    u64 id = event_id;
    id = id << 32 | tid;

    saved_args = args_map.lookup(&id);
    if (saved_args == 0) {
        // missed entry or not a container
        return -1;
    }

    args->args[0] = saved_args->args[0];
    args->args[1] = saved_args->args[1];
    args->args[2] = saved_args->args[2];
    args->args[3] = saved_args->args[3];
    args->args[4] = saved_args->args[4];
    args->args[5] = saved_args->args[5];

    if (delete)
        args_map.delete(&id);

    return 0;
}

static __always_inline int del_args(u32 event_id)
{
    u32 tid = bpf_get_current_pid_tgid();
    u64 id = event_id;
    id = id << 32 | tid;

    args_map.delete(&id);

    return 0;
}

#define ENC_ARG(n, arg) arg<<(8*n)
#define ENC_ARG0(arg) ENC_ARG(0, arg)
#define ENC_ARG1(arg) ENC_ARG(1, arg)
#define ENC_ARG2(arg) ENC_ARG(2, arg)
#define ENC_ARG3(arg) ENC_ARG(3, arg)
#define ENC_ARG4(arg) ENC_ARG(4, arg)
#define ENC_ARG5(arg) ENC_ARG(5, arg)
#define ENC_ARGS0()                                   0
#define ENC_ARGS1(arg0)                               ENC_ARG0(arg0)
#define ENC_ARGS2(arg0, arg1)                         ENC_ARG0(arg0)|ENC_ARG1(arg1)
#define ENC_ARGS3(arg0, arg1, arg2)                   ENC_ARG0(arg0)|ENC_ARG1(arg1)|ENC_ARG2(arg2)
#define ENC_ARGS4(arg0, arg1, arg2, arg3)             ENC_ARG0(arg0)|ENC_ARG1(arg1)|ENC_ARG2(arg2)|ENC_ARG3(arg3)
#define ENC_ARGS5(arg0, arg1, arg2, arg3, arg4)       ENC_ARG0(arg0)|ENC_ARG1(arg1)|ENC_ARG2(arg2)|ENC_ARG3(arg3)|ENC_ARG4(arg4)
#define ENC_ARGS6(arg0, arg1, arg2, arg3, arg4, arg5) ENC_ARG0(arg0)|ENC_ARG1(arg1)|ENC_ARG2(arg2)|ENC_ARG3(arg3)|ENC_ARG4(arg4)|ENC_ARG5(arg5)
#define DEC_ARG(n, enc_arg) ((enc_arg>>(8*n))&0xFF)

static __always_inline int get_encoded_arg_num(u64 types)
{
    unsigned int i, argnum = 0;
    #pragma unroll
    for(i=0; i<6; i++)
    {
        if (DEC_ARG(i, types) != NONE_T)
            argnum++;
    }
    return argnum;
}

static __always_inline int save_args_to_submit_buf(u64 types, u64 tags, args_t *args)
{
    unsigned int i;
    short family = 0;

    if (types == 0)
        return 0;

    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    #pragma unroll
    for(i=0; i<6; i++)
    {
        u8 tag = DEC_ARG(i, tags);
        switch (DEC_ARG(i, types))
        {
            case NONE_T:
                break;
            case INT_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(int), INT_T, tag);
                break;
            case UINT_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(unsigned int), UINT_T, tag);
                break;
            case OFF_T_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(off_t), OFF_T_T, tag);
                break;
            case DEV_T_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(dev_t), DEV_T_T, tag);
                break;
            case MODE_T_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(mode_t), MODE_T_T, tag);
                break;
            case LONG_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(long), LONG_T, tag);
                break;
            case ULONG_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(unsigned long), ULONG_T, tag);
                break;
            case SIZE_T_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(size_t), SIZE_T_T, tag);
                break;
            case POINTER_T:
                save_to_submit_buf(submit_p, (void*)&(args->args[i]), sizeof(void*), POINTER_T, tag);
                break;
            case STR_T:
                save_str_to_buf(submit_p, (void *)args->args[i], tag);
                break;
            case SOCKADDR_T:
                if (args->args[i]) {
                    bpf_probe_read(&family, sizeof(short), (void*)args->args[i]);
                    switch (family)
                    {
                        case AF_UNIX:
                            save_to_submit_buf(submit_p, (void*)(args->args[i]), sizeof(struct sockaddr_un), SOCKADDR_T, tag);
                            break;
                        case AF_INET:
                            save_to_submit_buf(submit_p, (void*)(args->args[i]), sizeof(struct sockaddr_in), SOCKADDR_T, tag);
                            break;
                        case AF_INET6:
                            save_to_submit_buf(submit_p, (void*)(args->args[i]), sizeof(struct sockaddr_in6), SOCKADDR_T, tag);
                            break;
                        default:
                            save_to_submit_buf(submit_p, (void*)&family, sizeof(short), SOCKADDR_T, tag);
                    }
                } else {
                    save_to_submit_buf(submit_p, (void*)(args->args[i]), 0, SOCKADDR_T, tag);
                }
                break;
        }
    }

    return 0;
}

static __always_inline int trace_ret_generic(struct pt_regs *ctx, u32 id, u64 types, u64 tags, bool delete_args)
{
    context_t context = {};
    args_t args = {};

    if (load_args(&args, delete_args, id) != 0)
        return -1;

    if (!should_trace())
        return -1;

    if (!event_chosen(id))
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = id;
    context.argnum = get_encoded_arg_num(types);
    context.retval = PT_REGS_RC(ctx);
    save_context_to_buf(submit_p, (void*)&context);
    save_args_to_submit_buf(types, tags, &args);

    events_perf_submit(ctx);
    return 0;
}

static __always_inline int trace_ret_generic_tail(struct pt_regs *ctx, u32 id, u64 types, u64 tags, u32 tail)
{
    bool delete_args = false;
    trace_ret_generic(ctx, id, types, tags, delete_args);
    prog_array.call(ctx, tail);
    // If we got here, tail call failed - delete args
    del_args(id);
    return 0;
}

static __always_inline int trace_ret_generic_fork(struct pt_regs *ctx, u32 id, u64 types, u64 tags)
{
    bool delete_args = true;
    int rc = trace_ret_generic(ctx, id, types, tags, delete_args);

    if (!rc && (get_config(CONFIG_MODE) != MODE_CONTAINER)) {
        u32 pid = PT_REGS_RC(ctx);
        add_pid_fork(pid);
    }

    return 0;
}

#define TRACE_ENT_SYSCALL(name, id)                                     \
int syscall__##name(struct pt_regs *ctx)                                \
{                                                                       \
    if (!should_trace())                                                \
        return 0;                                                       \
    return save_args(ctx, id, true);                                    \
}

#define TRACE_ENT_FUNC(name, id)                                        \
int trace_##name(struct pt_regs *ctx)                                   \
{                                                                       \
    if (!should_trace())                                                \
        return 0;                                                       \
    return save_args(ctx, id, false);                                   \
}

#define TRACE_RET_FUNC(name, id, types, tags)                           \
int trace_ret_##name(struct pt_regs *ctx)                               \
{                                                                       \
    bool delete_args = true;                                            \
    return trace_ret_generic(ctx, id, types, tags, delete_args);        \
}

#define TRACE_RET_FUNC_WITH_TAIL(name, id, types, tags, tail)           \
int trace_ret_##name(struct pt_regs *ctx)                               \
{                                                                       \
    return trace_ret_generic_tail(ctx, id, types, tags, tail);          \
}

#define TRACE_RET_SYSCALL TRACE_RET_FUNC
#define TRACE_RET_SYSCALL_WITH_TAIL TRACE_RET_FUNC_WITH_TAIL

#define TRACE_RET_FORK_SYSCALL(name, id, types, tags)                   \
int trace_ret_##name(struct pt_regs *ctx)                               \
{                                                                       \
    return trace_ret_generic_fork(ctx, id, types, tags);                \
}

/*============================== SYSCALL HOOKS ==============================*/

// Note: race condition may occur if a malicious user changes memory content pointed by syscall arguments by concurrent threads!
// Consider using inner kernel functions (e.g. security_file_open) to avoid this
TRACE_ENT_SYSCALL(open, SYS_OPEN);
TRACE_RET_SYSCALL(open, SYS_OPEN, ENC_ARGS2(STR_T, INT_T), ENC_ARGS2(TAG_PATHNAME, TAG_FLAGS));
TRACE_ENT_SYSCALL(openat, SYS_OPENAT);
TRACE_RET_SYSCALL(openat, SYS_OPENAT, ENC_ARGS3(INT_T, STR_T, INT_T), ENC_ARGS3(TAG_DIRFD, TAG_PATHNAME, TAG_FLAGS));
TRACE_ENT_SYSCALL(creat, SYS_CREAT);
TRACE_RET_SYSCALL(creat, SYS_CREAT, ENC_ARGS2(STR_T, INT_T), ENC_ARGS2(TAG_PATHNAME, TAG_MODE));
TRACE_ENT_SYSCALL(mmap, SYS_MMAP);
TRACE_RET_SYSCALL(mmap, SYS_MMAP, ENC_ARGS6(POINTER_T, SIZE_T_T, INT_T, INT_T, INT_T, OFF_T_T), ENC_ARGS6(TAG_ADDR, TAG_LENGTH, TAG_PROT, TAG_FLAGS, TAG_FD, TAG_OFFSET));
TRACE_ENT_SYSCALL(munmap, SYS_MUNMAP);
TRACE_RET_SYSCALL(munmap, SYS_MUNMAP, ENC_ARGS2(POINTER_T, SIZE_T_T), ENC_ARGS2(TAG_ADDR, TAG_LENGTH));
TRACE_ENT_SYSCALL(mprotect, SYS_MPROTECT);
TRACE_RET_SYSCALL(mprotect, SYS_MPROTECT, ENC_ARGS3(POINTER_T, SIZE_T_T, INT_T), ENC_ARGS3(TAG_ADDR, TAG_LENGTH, TAG_PROT));
TRACE_ENT_SYSCALL(brk, SYS_BRK);
TRACE_RET_SYSCALL(brk, SYS_BRK, ENC_ARGS1(POINTER_T), ENC_ARGS1(TAG_ADDR));
TRACE_ENT_SYSCALL(pkey_mprotect, SYS_PKEY_MPROTECT);
TRACE_RET_SYSCALL(pkey_mprotect, SYS_PKEY_MPROTECT, ENC_ARGS4(POINTER_T, SIZE_T_T, INT_T, INT_T), ENC_ARGS4(TAG_ADDR, TAG_LENGTH, TAG_PROT, TAG_PKEY));
TRACE_ENT_SYSCALL(mknod, SYS_MKNOD);
TRACE_RET_SYSCALL(mknod, SYS_MKNOD, ENC_ARGS3(STR_T, MODE_T_T, DEV_T_T), ENC_ARGS3(TAG_PATHNAME, TAG_MODE, TAG_DEV));
TRACE_ENT_SYSCALL(mknodat, SYS_MKNODAT);
TRACE_RET_SYSCALL(mknodat, SYS_MKNODAT, ENC_ARGS4(INT_T, STR_T, MODE_T_T, DEV_T_T), ENC_ARGS4(TAG_DIRFD, TAG_PATHNAME, TAG_MODE, TAG_DEV));
TRACE_ENT_SYSCALL(memfd_create, SYS_MEMFD_CREATE);
TRACE_RET_SYSCALL(memfd_create, SYS_MEMFD_CREATE, ENC_ARGS2(STR_T, INT_T), ENC_ARGS2(TAG_NAME, TAG_FLAGS));
TRACE_ENT_SYSCALL(dup, SYS_DUP);
TRACE_RET_SYSCALL(dup, SYS_DUP, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_OLDFD));
TRACE_ENT_SYSCALL(dup2, SYS_DUP2);
TRACE_RET_SYSCALL(dup2, SYS_DUP2, ENC_ARGS2(INT_T, INT_T), ENC_ARGS2(TAG_OLDFD, TAG_NEWFD));
TRACE_ENT_SYSCALL(dup3, SYS_DUP3);
TRACE_RET_SYSCALL(dup3, SYS_DUP3, ENC_ARGS3(INT_T, INT_T, INT_T), ENC_ARGS3(TAG_OLDFD, TAG_NEWFD, TAG_FLAGS));
TRACE_ENT_SYSCALL(newstat, SYS_STAT);
TRACE_RET_SYSCALL(newstat, SYS_STAT, ENC_ARGS1(STR_T), ENC_ARGS1(TAG_PATHNAME));
TRACE_ENT_SYSCALL(newlstat, SYS_LSTAT);
TRACE_RET_SYSCALL(newlstat, SYS_LSTAT, ENC_ARGS1(STR_T), ENC_ARGS1(TAG_PATHNAME));
TRACE_ENT_SYSCALL(newfstat, SYS_FSTAT);
TRACE_RET_SYSCALL(newfstat, SYS_FSTAT, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_FD));
TRACE_ENT_SYSCALL(socket, SYS_SOCKET);
TRACE_RET_SYSCALL(socket, SYS_SOCKET, ENC_ARGS3(INT_T, INT_T, INT_T), ENC_ARGS3(TAG_DOMAIN, TAG_TYPE, TAG_PROTOCOL));
TRACE_ENT_SYSCALL(close, SYS_CLOSE);
TRACE_RET_SYSCALL(close, SYS_CLOSE, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_FD));
TRACE_ENT_SYSCALL(ioctl, SYS_IOCTL);
TRACE_RET_SYSCALL(ioctl, SYS_IOCTL, ENC_ARGS2(INT_T, ULONG_T), ENC_ARGS2(TAG_FD, TAG_REQUEST));
TRACE_ENT_SYSCALL(access, SYS_ACCESS);
TRACE_RET_SYSCALL(access, SYS_ACCESS, ENC_ARGS2(STR_T, INT_T), ENC_ARGS2(TAG_PATHNAME, TAG_MODE));
TRACE_ENT_SYSCALL(faccessat, SYS_FACCESSAT);
TRACE_RET_SYSCALL(faccessat, SYS_FACCESSAT, ENC_ARGS4(INT_T, STR_T, INT_T, INT_T), ENC_ARGS4(TAG_DIRFD, TAG_PATHNAME, TAG_MODE, TAG_FLAGS));
TRACE_ENT_SYSCALL(kill, SYS_KILL);
TRACE_RET_SYSCALL(kill, SYS_KILL, ENC_ARGS2(INT_T, INT_T), ENC_ARGS2(TAG_PID, TAG_SIG));
TRACE_ENT_SYSCALL(listen, SYS_LISTEN);
TRACE_RET_SYSCALL(listen, SYS_LISTEN, ENC_ARGS2(INT_T, INT_T), ENC_ARGS2(TAG_SOCKFD, TAG_BACKLOG));
TRACE_ENT_SYSCALL(connect, SYS_CONNECT);
TRACE_RET_SYSCALL(connect, SYS_CONNECT, ENC_ARGS2(INT_T, SOCKADDR_T), ENC_ARGS2(TAG_SOCKFD, TAG_ADDR));
TRACE_ENT_SYSCALL(accept, SYS_ACCEPT);
TRACE_RET_SYSCALL(accept, SYS_ACCEPT, ENC_ARGS2(INT_T, SOCKADDR_T), ENC_ARGS2(TAG_SOCKFD, TAG_ADDR));
TRACE_ENT_SYSCALL(accept4, SYS_ACCEPT4);
TRACE_RET_SYSCALL(accept4, SYS_ACCEPT4, ENC_ARGS2(INT_T, SOCKADDR_T), ENC_ARGS2(TAG_SOCKFD, TAG_ADDR));
TRACE_ENT_SYSCALL(bind, SYS_BIND);
TRACE_RET_SYSCALL(bind, SYS_BIND, ENC_ARGS2(INT_T, SOCKADDR_T), ENC_ARGS2(TAG_SOCKFD, TAG_ADDR));
TRACE_ENT_SYSCALL(sendto, SYS_SENDTO);
TRACE_RET_SYSCALL(sendto, SYS_SENDTO, ENC_ARGS5(INT_T, POINTER_T, SIZE_T_T, INT_T, SOCKADDR_T), ENC_ARGS5(TAG_SOCKFD, TAG_BUF, TAG_LEN, TAG_FLAGS, TAG_DEST_ADDR));
TRACE_ENT_SYSCALL(recvfrom, SYS_RECVFROM);
TRACE_RET_SYSCALL(recvfrom, SYS_RECVFROM, ENC_ARGS5(INT_T, POINTER_T, SIZE_T_T, INT_T, SOCKADDR_T), ENC_ARGS5(TAG_SOCKFD, TAG_BUF, TAG_LEN, TAG_FLAGS, TAG_SRC_ADDR));
TRACE_ENT_SYSCALL(getsockname, SYS_GETSOCKNAME);
TRACE_RET_SYSCALL(getsockname, SYS_GETSOCKNAME, ENC_ARGS2(INT_T, SOCKADDR_T), ENC_ARGS2(TAG_SOCKFD, TAG_ADDR));
TRACE_ENT_SYSCALL(prctl, SYS_PRCTL);
TRACE_RET_SYSCALL(prctl, SYS_PRCTL, ENC_ARGS5(INT_T, ULONG_T, ULONG_T, ULONG_T, ULONG_T), ENC_ARGS5(TAG_OPTION, TAG_ARG2, TAG_ARG3, TAG_ARG4, TAG_ARG5));
TRACE_ENT_SYSCALL(ptrace, SYS_PTRACE);
TRACE_RET_SYSCALL(ptrace, SYS_PTRACE, ENC_ARGS4(INT_T, INT_T, POINTER_T, POINTER_T), ENC_ARGS4(TAG_REQUEST, TAG_PID, TAG_ADDR, TAG_DATA));
TRACE_ENT_SYSCALL(process_vm_writev, SYS_PROCESS_VM_WRITEV);
TRACE_RET_SYSCALL(process_vm_writev, SYS_PROCESS_VM_WRITEV, ENC_ARGS6(INT_T, POINTER_T, ULONG_T, POINTER_T, ULONG_T, ULONG_T), ENC_ARGS6(TAG_PID, TAG_LOCAL_IOV, TAG_LIOVCNT, TAG_REMOTE_IOV, TAG_RIOVCNT, TAG_FLAGS));
TRACE_ENT_SYSCALL(process_vm_readv, SYS_PROCESS_VM_READV);
TRACE_RET_SYSCALL(process_vm_readv, SYS_PROCESS_VM_READV, ENC_ARGS6(INT_T, POINTER_T, ULONG_T, POINTER_T, ULONG_T, ULONG_T), ENC_ARGS6(TAG_PID, TAG_LOCAL_IOV, TAG_LIOVCNT, TAG_REMOTE_IOV, TAG_RIOVCNT, TAG_FLAGS));
TRACE_ENT_SYSCALL(init_module, SYS_INIT_MODULE);
TRACE_RET_SYSCALL(init_module, SYS_INIT_MODULE, ENC_ARGS3(POINTER_T, ULONG_T, STR_T), ENC_ARGS3(TAG_MODULE_IMAGE, TAG_LEN, TAG_PARAM_VALUES));
TRACE_ENT_SYSCALL(finit_module, SYS_FINIT_MODULE);
TRACE_RET_SYSCALL(finit_module, SYS_FINIT_MODULE, ENC_ARGS3(INT_T, STR_T, INT_T), ENC_ARGS3(TAG_FD, TAG_PARAM_VALUES, TAG_FLAGS));
TRACE_ENT_SYSCALL(delete_module, SYS_DELETE_MODULE);
TRACE_RET_SYSCALL(delete_module, SYS_DELETE_MODULE, ENC_ARGS2(STR_T, INT_T), ENC_ARGS2(TAG_NAME, TAG_FLAGS));
TRACE_ENT_SYSCALL(symlink, SYS_SYMLINK);
TRACE_RET_SYSCALL(symlink, SYS_SYMLINK, ENC_ARGS2(STR_T, STR_T), ENC_ARGS2(TAG_TARGET, TAG_LINKPATH));
TRACE_ENT_SYSCALL(symlinkat, SYS_SYMLINKAT);
TRACE_RET_SYSCALL(symlinkat, SYS_SYMLINKAT, ENC_ARGS3(STR_T, INT_T, STR_T), ENC_ARGS3(TAG_TARGET, TAG_NEWDIRFD, TAG_LINKPATH));
TRACE_ENT_SYSCALL(getdents, SYS_GETDENTS);
TRACE_RET_SYSCALL(getdents, SYS_GETDENTS, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_FD));
TRACE_ENT_SYSCALL(getdents64, SYS_GETDENTS64);
TRACE_RET_SYSCALL(getdents64, SYS_GETDENTS64, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_FD));
TRACE_ENT_SYSCALL(mount, SYS_MOUNT);
TRACE_RET_SYSCALL(mount, SYS_MOUNT, ENC_ARGS4(STR_T, STR_T, STR_T, ULONG_T), ENC_ARGS4(TAG_SOURCE, TAG_TARGET, TAG_FILESYSTEMTYPE, TAG_MOUNTFLAGS));
TRACE_ENT_SYSCALL(umount, SYS_UMOUNT);
TRACE_RET_SYSCALL(umount, SYS_UMOUNT, ENC_ARGS1(STR_T), ENC_ARGS1(TAG_TARGET));
TRACE_ENT_SYSCALL(unlink, SYS_UNLINK);
TRACE_RET_SYSCALL(unlink, SYS_UNLINK, ENC_ARGS1(STR_T), ENC_ARGS1(TAG_PATHNAME));
TRACE_ENT_SYSCALL(unlinkat, SYS_UNLINKAT);
TRACE_RET_SYSCALL(unlinkat, SYS_UNLINKAT, ENC_ARGS3(INT_T, STR_T, INT_T), ENC_ARGS3(TAG_DIRFD, TAG_PATHNAME, TAG_FLAGS));
TRACE_ENT_SYSCALL(setuid, SYS_SETUID);
TRACE_RET_SYSCALL(setuid, SYS_SETUID, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_UID));
TRACE_ENT_SYSCALL(setgid, SYS_SETGID);
TRACE_RET_SYSCALL(setgid, SYS_SETGID, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_GID));
TRACE_ENT_SYSCALL(setfsuid, SYS_SETFSUID);
TRACE_RET_SYSCALL(setfsuid, SYS_SETFSUID, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_FSUID));
TRACE_ENT_SYSCALL(setfsgid, SYS_SETFSGID);
TRACE_RET_SYSCALL(setfsgid, SYS_SETFSGID, ENC_ARGS1(INT_T), ENC_ARGS1(TAG_FSGID));
TRACE_ENT_SYSCALL(setreuid, SYS_SETREUID);
TRACE_RET_SYSCALL(setreuid, SYS_SETREUID, ENC_ARGS2(INT_T, INT_T), ENC_ARGS2(TAG_RUID, TAG_EUID));
TRACE_ENT_SYSCALL(setregid, SYS_SETREGID);
TRACE_RET_SYSCALL(setregid, SYS_SETREGID, ENC_ARGS2(INT_T, INT_T), ENC_ARGS2(TAG_RGID, TAG_EGID));
TRACE_ENT_SYSCALL(setresuid, SYS_SETRESUID);
TRACE_RET_SYSCALL(setresuid, SYS_SETRESUID, ENC_ARGS3(INT_T, INT_T, INT_T), ENC_ARGS3(TAG_RUID, TAG_EUID, TAG_SUID));
TRACE_ENT_SYSCALL(setresgid, SYS_SETRESGID);
TRACE_RET_SYSCALL(setresgid, SYS_SETRESGID, ENC_ARGS3(INT_T, INT_T, INT_T), ENC_ARGS3(TAG_RGID, TAG_EGID, TAG_SGID));
TRACE_ENT_SYSCALL(chown, SYS_CHOWN);
TRACE_RET_SYSCALL(chown, SYS_CHOWN, ENC_ARGS3(STR_T, UINT_T, UINT_T), ENC_ARGS3(TAG_PATHNAME, TAG_OWNER, TAG_GROUP));
TRACE_ENT_SYSCALL(fchown, SYS_FCHOWN);
TRACE_RET_SYSCALL(fchown, SYS_FCHOWN, ENC_ARGS3(INT_T, UINT_T, UINT_T), ENC_ARGS3(TAG_FD, TAG_OWNER, TAG_GROUP));
TRACE_ENT_SYSCALL(lchown, SYS_LCHOWN);
TRACE_RET_SYSCALL(lchown, SYS_LCHOWN, ENC_ARGS3(STR_T, UINT_T, UINT_T), ENC_ARGS3(TAG_PATHNAME, TAG_OWNER, TAG_GROUP));
TRACE_ENT_SYSCALL(fchownat, SYS_FCHOWNAT);
TRACE_RET_SYSCALL(fchownat, SYS_FCHOWNAT, ENC_ARGS5(INT_T, STR_T, UINT_T, UINT_T, INT_T), ENC_ARGS5(TAG_DIRFD, TAG_PATHNAME, TAG_OWNER, TAG_GROUP, TAG_FLAGS));
TRACE_ENT_SYSCALL(chmod, SYS_CHMOD);
TRACE_RET_SYSCALL(chmod, SYS_CHMOD, ENC_ARGS2(STR_T, MODE_T_T), ENC_ARGS2(TAG_PATHNAME, TAG_MODE));
TRACE_ENT_SYSCALL(fchmod, SYS_FCHMOD);
TRACE_RET_SYSCALL(fchmod, SYS_FCHMOD, ENC_ARGS2(INT_T, MODE_T_T), ENC_ARGS2(TAG_FD, TAG_MODE));
TRACE_ENT_SYSCALL(fchmodat, SYS_FCHMODAT);
TRACE_RET_SYSCALL(fchmodat, SYS_FCHMODAT, ENC_ARGS4(INT_T, STR_T, MODE_T_T, INT_T), ENC_ARGS4(TAG_DIRFD, TAG_PATHNAME, TAG_MODE, TAG_FLAGS));
TRACE_ENT_SYSCALL(read, SYS_READ);
TRACE_RET_SYSCALL(read, SYS_READ, ENC_ARGS3(INT_T, POINTER_T, SIZE_T_T), ENC_ARGS3(TAG_FD, TAG_BUF, TAG_COUNT));
TRACE_ENT_SYSCALL(write, SYS_WRITE);
TRACE_RET_SYSCALL(write, SYS_WRITE, ENC_ARGS3(INT_T, POINTER_T, SIZE_T_T), ENC_ARGS3(TAG_FD, TAG_BUF, TAG_COUNT));
TRACE_ENT_SYSCALL(pread64, SYS_PREAD64);
TRACE_RET_SYSCALL(pread64, SYS_PREAD64, ENC_ARGS4(INT_T, POINTER_T, SIZE_T_T, OFF_T_T), ENC_ARGS4(TAG_FD, TAG_BUF, TAG_COUNT, TAG_OFFSET));
TRACE_ENT_SYSCALL(pwrite64, SYS_PWRITE64);
TRACE_RET_SYSCALL(pwrite64, SYS_PWRITE64, ENC_ARGS4(INT_T, POINTER_T, SIZE_T_T, OFF_T_T), ENC_ARGS4(TAG_FD, TAG_BUF, TAG_COUNT, TAG_OFFSET));
TRACE_ENT_SYSCALL(lseek, SYS_LSEEK);
TRACE_RET_SYSCALL(lseek, SYS_LSEEK, ENC_ARGS3(INT_T, OFF_T_T, INT_T), ENC_ARGS3(TAG_FD, TAG_OFFSET, TAG_WHENCE));
TRACE_ENT_SYSCALL(sched_yield, SYS_SCHED_YIELD);
TRACE_RET_SYSCALL(sched_yield, SYS_SCHED_YIELD, ENC_ARGS0(), ENC_ARGS0());
TRACE_ENT_SYSCALL(msync, SYS_MSYNC);
TRACE_RET_SYSCALL(msync, SYS_MSYNC, ENC_ARGS3(POINTER_T, SIZE_T_T, INT_T), ENC_ARGS3(TAG_ADDR, TAG_LENGTH, TAG_FLAGS));
TRACE_ENT_SYSCALL(madvise, SYS_MADVISE);
TRACE_RET_SYSCALL(madvise, SYS_MADVISE, ENC_ARGS3(POINTER_T, SIZE_T_T, INT_T), ENC_ARGS3(TAG_ADDR, TAG_LENGTH, TAG_ADVICE));
TRACE_ENT_SYSCALL(getpid, SYS_GETPID);
TRACE_RET_SYSCALL(getpid, SYS_GETPID, ENC_ARGS0(), ENC_ARGS0());
TRACE_ENT_SYSCALL(getppid, SYS_GETPPID);
TRACE_RET_SYSCALL(getppid, SYS_GETPPID, ENC_ARGS0(), ENC_ARGS0());
TRACE_ENT_SYSCALL(gettid, SYS_GETTID);
TRACE_RET_SYSCALL(gettid, SYS_GETTID, ENC_ARGS0(), ENC_ARGS0());

TRACE_ENT_SYSCALL(fork, SYS_FORK);
TRACE_RET_FORK_SYSCALL(fork, SYS_FORK, ENC_ARGS0(), ENC_ARGS0());
TRACE_ENT_SYSCALL(vfork, SYS_VFORK);
TRACE_RET_FORK_SYSCALL(vfork, SYS_VFORK, ENC_ARGS0(), ENC_ARGS0());
TRACE_ENT_SYSCALL(clone, SYS_CLONE);
TRACE_RET_FORK_SYSCALL(clone, SYS_CLONE, ENC_ARGS1(ULONG_T), ENC_ARGS1(TAG_FLAGS));

TRACEPOINT_PROBE(raw_syscalls, sys_enter) {
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = RAW_SYSCALLS;
    context.argnum = 1;
    context.retval = 0;

    save_context_to_buf(submit_p, (void*)&context);

    save_to_submit_buf(submit_p, (void*)&(args->id), sizeof(int), INT_T, TAG_SYSCALL);
    events_perf_submit((struct pt_regs *)args);
    
    return 0;
}

int syscall__execve(struct pt_regs *ctx,
    const char __user *filename,
    const char __user *const __user *__argv,
    const char __user *const __user *__envp)
{
    context_t context = {};

    if (get_config(CONFIG_MODE) == MODE_CONTAINER)
        add_pid_ns_if_needed();
    else if (get_config(CONFIG_MODE) == MODE_SYSTEM)
        add_pid();

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    int show_env = get_config(CONFIG_EXEC_ENV);

    context.eventid = SYS_EXECVE;
    if (show_env)
        context.argnum = 3;
    else
        context.argnum = 2;
    context.retval = 0;     // assume execve succeeded. if not, a ret event will be sent too
    save_context_to_buf(submit_p, (void*)&context);

    save_str_to_buf(submit_p, (void *)filename, TAG_PATHNAME);
    save_str_arr_to_buf(submit_p, __argv, TAG_ARGV);
    if (show_env)
        save_str_arr_to_buf(submit_p, __envp, TAG_ENVP);

    events_perf_submit(ctx);
    return 0;
}

int trace_ret_execve(struct pt_regs *ctx)
{
    // we can't load string args here as after execve memory is wiped
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = SYS_EXECVE;
    context.argnum = 0;
    context.retval = PT_REGS_RC(ctx);

    if (context.retval == 0)
        return 0;   // we are only interested in failed execs

    save_context_to_buf(submit_p, (void*)&context);
    events_perf_submit(ctx);
    return 0;
}

int syscall__execveat(struct pt_regs *ctx,
    const int dirfd,
    const char __user *pathname,
    const char __user *const __user *__argv,
    const char __user *const __user *__envp,
    const int flags)
{
    context_t context = {};

    if (get_config(CONFIG_MODE) == MODE_CONTAINER)
        add_pid_ns_if_needed();
    else if (get_config(CONFIG_MODE) == MODE_SYSTEM)
        add_pid();

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    int show_env = get_config(CONFIG_EXEC_ENV);

    context.eventid = SYS_EXECVEAT;
    if (show_env)
        context.argnum = 5;
    else
        context.argnum = 4;
    context.retval = 0;     // assume execve succeeded. if not, a ret event will be sent too
    save_context_to_buf(submit_p, (void*)&context);

    save_to_submit_buf(submit_p, (void*)&dirfd, sizeof(int), INT_T, TAG_DIRFD);
    save_str_to_buf(submit_p, (void *)pathname, TAG_PATHNAME);
    save_str_arr_to_buf(submit_p, __argv, TAG_ARGV);
    if (show_env)
        save_str_arr_to_buf(submit_p, __envp, TAG_ENVP);
    save_to_submit_buf(submit_p, (void*)&flags, sizeof(int), INT_T, TAG_FLAGS);

    events_perf_submit(ctx);
    return 0;
}

int trace_ret_execveat(struct pt_regs *ctx)
{
    // we can't load string args here as after execve memory is wiped
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = SYS_EXECVEAT;
    context.argnum = 0;
    context.retval = PT_REGS_RC(ctx);

    if (context.retval == 0)
        return 0;   // we are only interested in failed execs

    save_context_to_buf(submit_p, (void*)&context);
    events_perf_submit(ctx);
    return 0;
}

/*============================== OTHER HOOKS ==============================*/

int trace_do_exit(struct pt_regs *ctx, long code)
{
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = DO_EXIT;
    context.argnum = 0;
    context.retval = code;

    if (get_config(CONFIG_MODE) == MODE_CONTAINER)
        remove_pid_ns_if_needed();
    else
        remove_pid();

    save_context_to_buf(submit_p, (void*)&context);
    events_perf_submit(ctx);
    return 0;
}

int trace_security_bprm_check(struct pt_regs *ctx, struct linux_binprm *bprm)
{
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = SECURITY_BPRM_CHECK;
    context.argnum = 3;
    context.retval = 0;

    dev_t s_dev = bprm->file->f_inode->i_sb->s_dev;
    unsigned long inode_nr = (unsigned long)bprm->file->f_inode->i_ino;

    // Get per-cpu string buffer
    buf_t *string_p = get_buf(STRING_BUF_IDX);
    if (string_p == NULL)
        return -1;
    get_path_string(string_p, &bprm->file->f_path);

    save_context_to_buf(submit_p, (void*)&context);
    u32 *off = get_buf_off(STRING_BUF_IDX);
    if (off == NULL)
        return -1;
    save_str_to_buf(submit_p, (void *)&string_p->buf[*off], TAG_PATHNAME);
    save_to_submit_buf(submit_p, &s_dev, sizeof(dev_t), DEV_T_T, TAG_DEV);
    save_to_submit_buf(submit_p, &inode_nr, sizeof(unsigned long), ULONG_T, TAG_INODE);

    events_perf_submit(ctx);
    return 0;
}

int trace_security_file_open(struct pt_regs *ctx, struct file *file)
{
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = SECURITY_FILE_OPEN;
    context.argnum = 4;
    context.retval = 0;

    dev_t s_dev = file->f_inode->i_sb->s_dev;
    unsigned long inode_nr = (unsigned long)file->f_inode->i_ino;

    struct pt_regs *real_ctx = get_task_pt_regs();
    int syscall_nr = real_ctx->orig_ax;
    if (syscall_nr != 2 && syscall_nr != 257) // only monitor open and openat syscalls
        return 0;

    // Get per-cpu string buffer
    buf_t *string_p = get_buf(STRING_BUF_IDX);
    if (string_p == NULL)
        return -1;
    get_path_string(string_p, &file->f_path);

    save_context_to_buf(submit_p, (void*)&context);
    u32 *off = get_buf_off(STRING_BUF_IDX);
    if (off == NULL)
        return -1;
    save_str_to_buf(submit_p, (void *)&string_p->buf[*off], TAG_PATHNAME);
    save_to_submit_buf(submit_p, (void*)&file->f_flags, sizeof(int), INT_T, TAG_FLAGS);
    save_to_submit_buf(submit_p, &s_dev, sizeof(dev_t), DEV_T_T, TAG_DEV);
    save_to_submit_buf(submit_p, &inode_nr, sizeof(unsigned long), ULONG_T, TAG_INODE);

    events_perf_submit(ctx);
    return 0;
}

int trace_cap_capable(struct pt_regs *ctx, const struct cred *cred,
    struct user_namespace *targ_ns, int cap, int cap_opt)
{
    int audit;
    context_t context = {};

    if (!should_trace())
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = CAP_CAPABLE;
    if (get_config(CONFIG_SHOW_SYSCALL))
        context.argnum = 2;
    else
        context.argnum = 1;

  #ifdef CAP_OPT_NONE
    audit = (cap_opt & 0b10) == 0;
  #else
    audit = cap_opt;
  #endif

    if (audit == 0)
        return 0;

    save_context_to_buf(submit_p, (void*)&context);
    save_to_submit_buf(submit_p, (void*)&cap, sizeof(int), INT_T, TAG_CAP);
    if (get_config(CONFIG_SHOW_SYSCALL)) {
        struct pt_regs *real_ctx = get_task_pt_regs();
        save_to_submit_buf(submit_p, (void*)&(real_ctx->orig_ax), sizeof(int), INT_T, TAG_SYSCALL);
    }
    events_perf_submit(ctx);
    return 0;
};

int send_bin(struct pt_regs *ctx)
{
    // Note: sending the data to the userspace have the following constraints:
    // 1. We need a buffer that we know it's exact size (so we can send chunks of known sizes in BPF)
    // 2. We can have multiple cpus - need percpu array
    // 3. We have to use perf submit and not maps as data can be overriden if userspace doesn't consume it fast enough

    int i = 0;
    unsigned int chunk_size;

    u64 id = bpf_get_current_pid_tgid();

    bin_args_t *bin_args = bin_args_map.lookup(&id);
    if (bin_args == 0) {
        // missed entry or not traced
        return 0;
    }

    if (bin_args->full_size <= 0) {
        bin_args_map.delete(&id);
        return 0;
    }

    buf_t *file_buf_p = get_buf(FILE_BUF_IDX);
    if (file_buf_p == NULL) {
        bin_args_map.delete(&id);
        return 0;
    }

#define F_SEND_TYPE   0
#define F_MNT_NS      (F_SEND_TYPE + sizeof(u8))
#define F_META_OFF    (F_MNT_NS + sizeof(u32))
#define F_SZ_OFF      (F_META_OFF + SEND_META_SIZE)
#define F_POS_OFF     (F_SZ_OFF + sizeof(unsigned int))
#define F_CHUNK_OFF   (F_POS_OFF + sizeof(off_t))
#define F_CHUNK_SIZE  (MAX_PERCPU_BUFSIZE - F_CHUNK_OFF - 4)

    bpf_probe_read((void **)&(file_buf_p->buf[F_SEND_TYPE]), sizeof(u8), &bin_args->type);

    u32 mnt_id = get_task_mnt_ns_id((struct task_struct *)bpf_get_current_task());
    bpf_probe_read((void **)&(file_buf_p->buf[F_MNT_NS]), sizeof(u32), &mnt_id);

    // Save metadata to be used in filename
    bpf_probe_read((void **)&(file_buf_p->buf[F_META_OFF]), SEND_META_SIZE, bin_args->metadata);

    // Save number of written bytes. Set this to CHUNK_SIZE for full chunks
    chunk_size = F_CHUNK_SIZE;
    bpf_probe_read((void **)&(file_buf_p->buf[F_SZ_OFF]), sizeof(unsigned int), &chunk_size);

    unsigned int full_chunk_num = bin_args->full_size/F_CHUNK_SIZE;
    void *data = file_buf_p->buf;

    // Handle full chunks in loop
    #pragma unroll
    for (i = 0; i < 110; i++) {
        // Dummy instruction, as break instruction can't be first with unroll optimization
        chunk_size = F_CHUNK_SIZE;

        if (i == full_chunk_num)
            break;

        // Save binary chunk and file position of write
        bpf_probe_read((void **)&(file_buf_p->buf[F_POS_OFF]), sizeof(off_t), &bin_args->start_off);
        bpf_probe_read((void **)&(file_buf_p->buf[F_CHUNK_OFF]), F_CHUNK_SIZE, bin_args->ptr);
        bin_args->ptr += F_CHUNK_SIZE;
        bin_args->start_off += F_CHUNK_SIZE;

        file_writes.perf_submit(ctx, data, F_CHUNK_OFF+F_CHUNK_SIZE);
    }

    chunk_size = bin_args->full_size - i*F_CHUNK_SIZE;

    if (chunk_size > F_CHUNK_SIZE) {
        bin_args_t bin_args_new = {};
        bin_args_new.type = bin_args->type;
        bin_args_new.ptr = bin_args->ptr;
        bin_args_new.start_off = bin_args->start_off;
        bin_args_new.full_size = chunk_size;
        bpf_probe_read((void *)(bin_args_new.metadata), SEND_META_SIZE, bin_args->metadata);
        bin_args_map.update(&id, &bin_args_new);

        // Handle the rest of the write recursively
        prog_array.call(ctx, TAIL_SEND_BIN);
        return 0;
    }

    // Save last chunk
    bpf_probe_read((void **)&(file_buf_p->buf[F_CHUNK_OFF]), chunk_size, bin_args->ptr);
    bpf_probe_read((void **)&(file_buf_p->buf[F_SZ_OFF]), sizeof(unsigned int), &chunk_size);
    bpf_probe_read((void **)&(file_buf_p->buf[F_POS_OFF]), sizeof(off_t), &bin_args->start_off);

    // Satisfy validator by setting buffer bounds
    int size = (F_CHUNK_OFF+chunk_size) & (MAX_PERCPU_BUFSIZE - 1);
    file_writes.perf_submit(ctx, data, size);

    bin_args_map.delete(&id);
    return 0;
}

TRACE_ENT_FUNC(vfs_write, VFS_WRITE);

int trace_ret_vfs_write(struct pt_regs *ctx)
{
    struct path path;
    args_t saved_args;
    bool has_filter = false;

    bool delete_args = false;
    if (load_args(&saved_args, delete_args, VFS_WRITE) != 0) {
        // missed entry or not traced
        return 0;
    }

    struct file *file      = (struct file *) saved_args.args[0];

    // Extract path of written file
    bpf_probe_read(&path, sizeof(struct path), &file->f_path);
    // Get per-cpu string buffer
    buf_t *string_p = get_buf(STRING_BUF_IDX);
    if (string_p == NULL)
        return -1;
    get_path_string(string_p, &path);
    u32 *off = get_buf_off(STRING_BUF_IDX);
    if (off == NULL)
        return -1;

    #pragma unroll
    for (int i = 0; i < 3; i++) {
        int idx = i;
        path_filter_t *filter_p = file_filter.lookup(&idx);
        if (filter_p == NULL)
            return -1;

        if (!filter_p->path[0])
            break;

        has_filter = true;

        if (*off > MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE)
            break;

        if (filter_p->path[0] && is_prefix(filter_p->path, &string_p->buf[*off]))
            prog_array.call(ctx, TAIL_VFS_WRITE);
    }

    if (has_filter) {
        // There is a filter, but no match
        del_args(VFS_WRITE);
        return 0;
    }

    // No filter was given - continue
    prog_array.call(ctx, TAIL_VFS_WRITE);
    return 0;
}

int do_trace_ret_vfs_write(struct pt_regs *ctx)
{
    context_t context = {};
    args_t saved_args;
    bin_args_t bin_args = {};
    struct inode *inode;
    struct super_block *superblock;
    dev_t s_dev;
    unsigned long inode_nr;
    unsigned int i_mode;
    loff_t start_pos;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    bool delete_args = true;
    if (load_args(&saved_args, delete_args, VFS_WRITE) != 0) {
        // missed entry or not traced
        return 0;
    }

    struct file *file      = (struct file *) saved_args.args[0];
    void *ptr              = (void*)         saved_args.args[1];
    size_t count           = (size_t)        saved_args.args[2];
    loff_t *pos            = (loff_t*)       saved_args.args[3];

    // Get path
    buf_t *string_p = get_buf(STRING_BUF_IDX);
    if (string_p == NULL)
        return -1;
    u32 *off = get_buf_off(STRING_BUF_IDX);
    if (off == NULL)
        return -1;

    // Extract device id, inode number, mode, and pos (offset)
    bpf_probe_read(&inode, sizeof(struct inode *), &file->f_inode);
    bpf_probe_read(&superblock, sizeof(struct super_block *), &inode->i_sb);
    bpf_probe_read(&s_dev, sizeof(dev_t), &superblock->s_dev);
    bpf_probe_read(&inode_nr, sizeof(unsigned long), &inode->i_ino);
    bpf_probe_read(&i_mode, sizeof(unsigned short), &inode->i_mode);
    bpf_probe_read(&start_pos, sizeof(off_t), pos);

    // Calculate write start offset
    if (start_pos != 0)
        start_pos -= PT_REGS_RC(ctx);

    context.eventid = VFS_WRITE;
    context.argnum = 5;
    context.retval = PT_REGS_RC(ctx);
    save_context_to_buf(submit_p, &context);

    save_str_to_buf(submit_p, (void *)&string_p->buf[*off], TAG_PATHNAME);
    save_to_submit_buf(submit_p, &s_dev, sizeof(dev_t), DEV_T_T, TAG_DEV);
    save_to_submit_buf(submit_p, &inode_nr, sizeof(unsigned long), ULONG_T, TAG_INODE);
    save_to_submit_buf(submit_p, &count, sizeof(size_t), SIZE_T_T, TAG_COUNT);
    save_to_submit_buf(submit_p, &start_pos, sizeof(off_t), OFF_T_T, TAG_POS);

    // Submit vfs_write event
    events_perf_submit(ctx);

    u64 id = bpf_get_current_pid_tgid();
    u32 pid = context.pid;
    if (*off > MAX_PERCPU_BUFSIZE - MAX_STRING_SIZE)
        return -1;
    if (!is_prefix("/dev/null", &string_p->buf[*off]))
        pid = 0;

    if (get_config(CONFIG_CAPTURE_FILES)) {
        bin_args.type = SEND_VFS_WRITE;
        bpf_probe_read(bin_args.metadata, 4, &s_dev);
        bpf_probe_read(&bin_args.metadata[4], 8, &inode_nr);
        bpf_probe_read(&bin_args.metadata[12], 4, &i_mode);
        bpf_probe_read(&bin_args.metadata[16], 4, &pid);
        bin_args.ptr = ptr;
        bin_args.start_off = start_pos;
        bin_args.full_size = PT_REGS_RC(ctx);
        bin_args_map.update(&id, &bin_args);

        // Send file data
        prog_array.call(ctx, TAIL_SEND_BIN);
    }
    return 0;
}

int trace_mmap_alert(struct pt_regs *ctx)
{
    context_t context = {};
    args_t args = {};

    // Workaround to a bug in gobpf, where a kprobe event (e.g. mmap syscall) can't be attached to two different functions.
    // As we need to use save_args in kprobe entry, but one of the events (functions) might be chosen and the other not,
    // We need a mechanism that can tell if an event was chosen by the user, so we can save the args without enabling both events.
    // Don't delete args if mmap event is selected, as it will be used in mmap kretprobe
    bool delete_args = true;
    if (event_chosen(SYS_MMAP))
        delete_args = false;

    if (load_args(&args, delete_args, SYS_MMAP) != 0)
        return 0;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = MEM_PROT_ALERT;
    context.argnum = 1;
    context.retval = 0;
    save_context_to_buf(submit_p, (void*)&context);

    if ((args.args[2] & (VM_WRITE|VM_EXEC)) == (VM_WRITE|VM_EXEC)) {
        alert_t alert = {.ts = context.ts, .msg = ALERT_MMAP_W_X, .payload = 0};
        save_to_submit_buf(submit_p, &alert, sizeof(alert_t), ALERT_T, TAG_ALERT);
        events_perf_submit(ctx);
    }

    return 0;
}

int trace_mprotect_alert(struct pt_regs *ctx, struct vm_area_struct *vma, unsigned long reqprot, unsigned long prot)
{
    context_t context = {};
    args_t args = {};
    bin_args_t bin_args = {};

    // Workaround to a bug in gobpf, where a kprobe event (e.g. mmap syscall) can't be attached to two different functions.
    // As we need to use save_args in kprobe entry, but one of the events (functions) might be chosen and the other not,
    // We need a mechanism that can tell if an event was chosen by the user, so we can save the args without enabling both events.
    // Don't delete args if mprotect event is selected, as it will be used in mprotect kretprobe
    bool delete_args = true;
    if (event_chosen(SYS_MPROTECT))
        delete_args = false;

    if (load_args(&args, delete_args, SYS_MPROTECT) != 0)
        return 0;

    void *addr = (void*)args.args[0];
    size_t len = args.args[1];
    unsigned long prev_prot = vma->vm_flags;

    if (addr <= 0)
        return 0;

    // If length is 0, the current page permissions are changed
    if (len == 0)
        len = PAGE_SIZE;

    init_context(&context);
    set_buf_off(SUBMIT_BUF_IDX, sizeof(context_t));
    buf_t *submit_p = get_buf(SUBMIT_BUF_IDX);
    if (submit_p == NULL)
        return 0;

    context.eventid = MEM_PROT_ALERT;
    context.argnum = 1;
    context.retval = 0;
    save_context_to_buf(submit_p, (void*)&context);

    if ((!(prev_prot & VM_EXEC)) && (reqprot & VM_EXEC)) {
        alert_t alert = {.ts = context.ts, .msg = ALERT_MPROT_X_ADD, .payload = 0};
        save_to_submit_buf(submit_p, &alert, sizeof(alert_t), ALERT_T, TAG_ALERT);
        events_perf_submit(ctx);
        return 0;
    }

    if ((prev_prot & VM_EXEC) && !(prev_prot & VM_WRITE)
        && ((reqprot & (VM_WRITE|VM_EXEC)) == (VM_WRITE|VM_EXEC))) {
        alert_t alert = {.ts = context.ts, .msg = ALERT_MPROT_W_ADD, .payload = 0};
        save_to_submit_buf(submit_p, &alert, sizeof(alert_t), ALERT_T, TAG_ALERT);
        events_perf_submit(ctx);
        return 0;
    }

    if (((prev_prot & (VM_WRITE|VM_EXEC)) == (VM_WRITE|VM_EXEC))
        && (reqprot & VM_EXEC) && !(reqprot & VM_WRITE)) {
        alert_t alert = {.ts = context.ts, .msg = ALERT_MPROT_W_REM, .payload = 0 };
        if (get_config(CONFIG_EXTRACT_DYN_CODE)) 
            alert.payload = 1;
        save_to_submit_buf(submit_p, &alert, sizeof(alert_t), ALERT_T, TAG_ALERT);
        events_perf_submit(ctx);

        if (get_config(CONFIG_EXTRACT_DYN_CODE)) {
            bin_args.type = SEND_MPROTECT;
            bpf_probe_read(bin_args.metadata, sizeof(u64), &context.ts);
            bin_args.ptr = (char *)addr;
            bin_args.start_off = 0;
            bin_args.full_size = len;

            u64 id = bpf_get_current_pid_tgid();
            bin_args_map.update(&id, &bin_args);
            prog_array.call(ctx, TAIL_SEND_BIN);
        }
    }

    return 0;
}
