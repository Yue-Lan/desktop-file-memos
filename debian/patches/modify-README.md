Description: <short summary of the patch>
 TODO: Put a short summary on the line above and replace this paragraph
 with a longer explanation of this change. Complete the meta-information
 with other relevant fields (see below for details). To make it easier, the
 information below has been extracted from the changelog. Adjust it or drop
 it.
 .
 desktop-file-memos (1.0-2ubuntu1804) bionic; urgency=medium
 .
   * debain/control: Fixed the error spelling 'pl(a)tform'.
Author: Yue Lan <lanyue@kylinos.cn>

---
The information above should follow the Patch Tagging Guidelines, please
checkout http://dep.debian.net/deps/dep3/ to learn about the format. Here
are templates for supplementary fields that you might want to add:

Origin: <vendor|upstream|other>, <url of original patch>
Bug: <url in upstream bugtracker>
Bug-Debian: https://bugs.debian.org/<bugnumber>
Bug-Ubuntu: https://launchpad.net/bugs/<bugnumber>
Forwarded: <no|not-needed|url proving that it has been forwarded>
Reviewed-By: <name and email of someone who approved the patch>
Last-Update: 2019-04-28

--- desktop-file-memos-1.0.orig/README.md
+++ desktop-file-memos-1.0/README.md
@@ -3,12 +3,6 @@ A desktop classification app on Linux.
 
 See also [README_CN.md](https://github.com/Yue-Lan/desktop-file-memos/blob/master/README_CN.md)
 
-# installing on ubuntu
-It provides a ppa for ubuntu's installtion. After the installation is completed you need to start it manually, or log out and log in again. It will start automatically.
-- sudo add-apt-repository ppa:larue/desktop-file-memos
-- sudo apt-get update
-- sudo apt-get install desktop-file-memos
-
 # for gtk-theme
 There is a qt official plugins package which named 'qt5-gtk-platformtheme', this is a recommend package of this project. If your desktop is gtk-based and you want this application use your system theme. You need:
 
