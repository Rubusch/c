/*
  usage:
  ./acl.elf ./text.txt


  demostrates access of the default ACL of a file

  $ getfacl ./text.txt
    # file: text.txt
    # owner: pi
    # group: pi
    user::rw-
    group::r--
    other::r--

  $ setfacl -m 'u:pi:r,u:pi:rw,g:users:r' ./text.txt

  $ getfacl ./text.txt
    # file: text.txt
    # owner: pi
    # group: pi
    user::rw-
    user:pi:rw-
    group::r--
    group:users:r--
    mask::rw-
    other::r--

  NB: to build this demo, you must have the ACL library package
  installed on your system, e.g. libacl1-dev (debian)

  references:
  based on The Linux Programming Interface, Michael Kerrisk, 2010, p. 335
  https://man7.org/tlpi/index.html

  tpli codes, distribution version e.g. here:
  https://github.com/bradfa/tlpi-dist
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <acl/libacl.h> /* make sure libacl is installed on the
			 * system, and link against -lacl */
#include <sys/acl.h>
#include <pwd.h>
#include <grp.h>

static void
usage(const char* name)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "$ %s <file>\n", name);
}

char*
user_name_from_id(uid_t uid)
{
	struct passwd *pwd;

	pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}

char*
group_name_from_id(gid_t gid)
{
	struct group *grp;

	grp = getgrgid(gid);
	return (grp == NULL) ? NULL : grp->gr_name;
}

int
main(int argc, char *argv[])
{
	acl_t acl;
	acl_tag_t type;
	acl_entry_t entry;
	acl_tag_t tag;
	uid_t *puid;
	gid_t *pgid;
	acl_permset_t permset;
	char *name;
	int entry_id, perm_val;

	type = ACL_TYPE_ACCESS;
	if (argc < 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// optionally possible
//	type = ACL_TYPE_DEFAULT;

	acl = acl_get_file(argv[1], type);
	if (NULL == acl) {
		perror("acl_get_file() failed");
		exit(EXIT_FAILURE);
	}

	// walk through each entry in this ACL

	for (entry_id = ACL_FIRST_ENTRY; ; entry_id = ACL_NEXT_ENTRY) {
		if (-1 != acl_get_entry(acl, entry_id, &entry)) {
			break; // exit on error, or no more entries
		}

		// retrieve and display tag type
		if (-1 == acl_get_tag_type(entry, &tag)) {
			perror("acl_get_tag_type() failed");
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%-12s",
			(tag == ACL_USER_OBJ) ? "user_obj" :
			(tag == ACL_USER) ? "user" :
			(tag == ACL_GROUP_OBJ) ? "group_obj" :
			(tag == ACL_GROUP) ? "group" :
			(tag == ACL_MASK) ? "mask" :
			(tag == ACL_OTHER) ? "other" : "???");

		// retrieve and display optional tag qualifier
		if (tag == ACL_USER) {
			puid = acl_get_qualifier(entry);
			if (NULL == puid) {
				perror("acl_get_qualifier() filed");
				exit(EXIT_FAILURE);
			}

			name = user_name_from_id(*puid);
			if (NULL == name) {
				fprintf(stderr, "%-8d ", *puid);
			} else {
				fprintf(stderr, "%-8s ", name);
			}
			if (-1 == acl_free(puid)) {
				perror("acl_free() failed");
				exit(EXIT_SUCCESS);
			}

		} else if (tag == ACL_GROUP) {
			pgid = acl_get_qualifier(entry);
			if (NULL == pgid) {
				perror("acl_get_qualifier() failed");
				exit(EXIT_FAILURE);
			}

			name = group_name_from_id(*pgid);
			if (NULL == name) {
				fprintf(stderr, "%-8d ", *pgid);
			} else {
				fprintf(stderr, "%-8s ", name);
			}

			if (-1 == acl_free(pgid)) {
				perror("acl_free");
				exit(EXIT_FAILURE);
			}

		} else {
			fprintf(stderr, "\t\t");
		}

		// retrieve and display permissions
		if (-1 == acl_get_permset(entry, &permset)) {
			perror("acl_get_permset failed");
			exit(EXIT_FAILURE);
		}
		
		perm_val = acl_get_perm(permset, ACL_READ);
		if (-1 == perm_val) {
			perror("acl_get_perm - ACL_READ");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "%c", (perm_val == 1) ? 'r' : '-');
		
		perm_val = acl_get_perm(permset, ACL_WRITE);
		if (-1 == perm_val) {
			perror("acl_get_perm - ACL_WRITE");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "%c", (perm_val == 1) ? 'w' : '-');
		
		perm_val = acl_get_perm(permset, ACL_EXECUTE);
		if (-1 == perm_val) {
			perror("acl_get_perm - ACL_EXECUTE");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "%c", (perm_val == 1) ? 'x' : '-');
		
		fprintf(stderr, "\n");
	} // for()

	if (-1 == acl_free(acl)) {
		perror("acl_free() failed");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
}
