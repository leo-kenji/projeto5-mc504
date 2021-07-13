#include <linux/kernel.h>
#include <linux/syscalls.h>
// #include <linux/cred.h>
#include <linux/sched/user.h>
#include <linux/sched.h>
#include <linux/errno.h>

#define ROOT_UID 0

// https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
// #include <sys/types.h>
// #include <unistd.h>

// uid_t getuid (void);

// https://stackoverflow.com/questions/39229639/how-to-get-current-processs-uid-and-euid-in-linux-kernel-4-2/39230936

int my_get_uid(kuid_t *m_uid, int uid)
{
	if (uid == -1) {
		const struct cred *cred = current_cred();
		*m_uid = cred->uid;
		return 0;
	} else if (uid >= 0 && uid < 65535) {
		m_uid->val = uid;
		return 0;
	}
	return -1;
}

SYSCALL_DEFINE1(getuserweight, int, uid)
{
	kuid_t m_uid;
	if (my_get_uid(&m_uid, uid)) {
		return -EINVAL;
	}

	struct user_struct *teste;
	teste = find_user(m_uid);
	if (teste == NULL) {
		return -EINVAL;
	}

	return teste->priority;
}

SYSCALL_DEFINE2(setuserweight, int, uid, int, weight)
{
	// If is root
	const struct cred *cred = current_cred();
	if (cred->uid.val == ROOT_UID) {
		kuid_t m_uid;
		if (my_get_uid(&m_uid, uid))
			return -EINVAL;

		struct user_struct *teste;
		if (weight < 0) {
			return -EINVAL;
		}

		teste = find_user(m_uid);
		if (teste == NULL) {
			return -EINVAL;
		} else {
			teste->priority = weight;
		}
	} else {
		return -EACCES;
	}

	return 0;
}
