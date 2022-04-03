/*
   a demo to display all process user and group ideas

   based on: The Linux Programming Interface, Michael Kerrisk, 2010, p. 182

   @author: Lothar Rubusch
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>

#define SG_SIZE (NGROUPS_MAX + 1)

int main(int argc, char *argv[])
{
	uid_t ruid, euid, suid, fsuid;
	gid_t rgid, egid, sgid, fsgid;
	gid_t supplementary_groups[SG_SIZE];
	int ngroups, jdx;
	char *ptr;

	if (-1 == getresuid(&ruid, &euid, &suid)) {
		perror("getresuid failed");
		goto err;
	}
	if (-1 == getresgid(&rgid, &egid, &sgid)) {
		perror("getresgid failed");
		goto err;
	}

/*
    NB: attempts to change the file-system IDs are always ignored for
    unprivileged processes, but even so, the following calls return the
    current file-system IDs (M. Kerrisk)
*/

	fsuid = setfsuid(0);
	fsgid = setfsgid(0);

	fprintf(stderr, "UID: ");
	{
		struct passwd *pwd;
		pwd = getpwuid(ruid);
		ptr = (pwd == NULL) ? NULL : pwd->pw_name;
	}
	fprintf(stderr, "real=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) ruid);

	{
		struct passwd *pwd;
		pwd = getpwuid(euid);
		ptr = (pwd == NULL) ? NULL : pwd->pw_name;
	}
	fprintf(stderr, "eff=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) euid);

	{
		struct passwd *pwd;
		pwd = getpwuid(suid);
		ptr = (pwd == NULL) ? NULL : pwd->pw_name;
	}
	fprintf(stderr, "saved=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) suid);

	{
		struct passwd *pwd;
		pwd = getpwuid(fsuid);
		ptr = (pwd == NULL) ? NULL : pwd->pw_name;
	}
	fprintf(stderr, "fs=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) fsuid);

	fprintf(stderr, "\n");


	fprintf(stderr, "GID: ");
	{
		struct group *grp;
		grp = getgrgid(rgid);
		ptr = (grp == NULL) ? NULL : grp->gr_name;
	}
	fprintf(stderr, "real=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) rgid);

	{
		struct group *grp;
		grp = getgrgid(egid);
		ptr = (grp == NULL) ? NULL : grp->gr_name;
	}
	fprintf(stderr, "eff=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) egid);

	{
		struct group *grp;
		grp = getgrgid(sgid);
		ptr = (grp == NULL) ? NULL : grp->gr_name;
	}
	fprintf(stderr, "saved=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) sgid);

	{
		struct group *grp;
		grp = getgrgid(fsgid);
		ptr = (grp == NULL) ? NULL : grp->gr_name;
	}
	fprintf(stderr, "fs=%s (%ld); ",
			(ptr == NULL) ? "???" : ptr, (unsigned long) fsgid);

	fprintf(stderr, "\n");


	ngroups = getgroups(SG_SIZE, supplementary_groups);
	if (-1 == ngroups) {
		perror("getgroups failed");
		goto err;
	}

	fprintf(stderr, "supplementary groups (%d): ", ngroups);
	for (jdx = 0; jdx < ngroups; jdx++) {
		struct group *grp;
		grp = getgrgid(supplementary_groups[jdx]);
		ptr = grp->gr_name;
		fprintf(stderr, "%s (%lu) ",
				(ptr == NULL) ? "???" : ptr, (unsigned long) supplementary_groups);
	}
	fprintf(stderr, "\n");


	fprintf(stderr, "READY.\n");
	exit(EXIT_SUCCESS);
err:
	exit(EXIT_FAILURE);
}
