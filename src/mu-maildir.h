/* 
** Copyright (C) 2008-2010 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 3, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation,
** Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  
**  
*/

#ifndef __MU_MAILDIR_H__
#define __MU_MAILDIR_H__

#include <glib.h>
#include <time.h>
#include <sys/types.h>          /* for mode_t */
#include <mu-util.h> 
#include <mu-msg-flags.h>


G_BEGIN_DECLS

/**
 * create a new maildir. Note, if the function fails 'halfway', it
 * will *not* try to remove the parts the were created. it *will*
 * create any parent dirs that are not yet existant.
 * 
 * 
 * @param path the path (missing components will be created, as in 'mkdir -p')
 * @param mode the file mode (e.g., 0755)
 * @param noindex add a .noindex file to the maildir, so it will be excluded
 * from indexing by 'mu index'
 * @param err if function returns FALSE, err may contain extra
 * information. if err is NULL, does nothing
 * 
 * @return TRUE if creation succeeded, FALSE otherwise
 */
gboolean mu_maildir_mkdir (const char* path, mode_t mode, gboolean noindex,
			   GError **err);


/**
 * create a symbolic link to a mail message
 * 
 * @param src the full path to the source message
 * @param targetpath the path to the target maildir; ie., *not*
 * MyMaildir/cur, but just MyMaildir/. The function will figure out
 * the correct subdir then. 
 * @param err if function returns FALSE, err may contain extra
 * information. if err is NULL, does nothing
 * 
 * @return 
 */
gboolean mu_maildir_link   (const char* src, const char *targetpath, GError **err);

/**
 * MuMaildirWalkMsgCallback -- callback function for
 * mu_path_walk_maildir; see the documentation there. It will be
 * called for each message found, with fullpath containing the full
 * path to the message, mdir containing the maildir -- that is, when
 * indexing ~/Maildir, a message ~/Maildir/foo/bar/cur/msg would have
 * the maildir "foo/bar". Then, the information from 'stat' of this
 * file (see stat(3)), and a user_data pointer
 */
typedef MuResult (*MuMaildirWalkMsgCallback)
(const char* fullpath, const char* mdir, struct stat *statinfo, void *user_data);

/**
 * MuPathWalkDirCallback -- callback function for mu_path_walk_maildir; see the
 * documentation there. It will be called each time a dir is entered or left,
 * with 'enter' being TRUE upon entering, FALSE otherwise 
 */
typedef MuResult (*MuMaildirWalkDirCallback)
     (const char* fullpath, gboolean enter, void *user_data);

/**
 * start a recursive walk of a maildir; for each file found, we call
 * callback with the path (with the Maildir path of scanner_new as
 * root), the filename, the timestamp (mtime) of the file,and the
 * *data pointer, for user data.  dot-files are ignored, as well as
 * files outside cur/ and new/ dirs and unreadable files; however,
 * dotdirs are visited (ie. '.dotdir/cur'), so this enables Maildir++.
 * (http://www.inter7.com/courierimap/README.maildirquota.html, search
 * for 'Mission statement'). In addition, dirs containing a file named
 * '.noindex' are ignored, as are their subdirectories.
 *
 * mu_walk_maildir stops if the callbacks return something different
 * from MU_OK. For example, it can return MU_STOP to stop the scan, or
 * some error.
 * 
 * @param path the maildir path to scan
 * @param cb_msg the callback function called for each msg
 * @param cb_dir the callback function called for each dir
 * @param data user data pointer
 * 
 * @return a scanner result; MU_OK if everything went ok, 
 * MU_STOP if we want to stop, or MU_ERROR in
 * case of error
 */
MuResult mu_maildir_walk (const char *path, MuMaildirWalkMsgCallback cb_msg, 
			  MuMaildirWalkDirCallback cb_dir, void *data);
/**
 * recursively delete all the symbolic links in a directory tree
 * 
 * @param dir top dir
 * @param err if function returns FALSE, err may contain extra
 * information. if err is NULL, does nothing
 * 
 * @return TRUE if it worked, FALSE in case of error
 */
gboolean mu_maildir_clear_links (const gchar* dir, GError **err);


/**
 * get the Maildir flags from the full path of a mailfile. The flags
 * are as specified in http://cr.yp.to/proto/maildir.html, plus
 * MU_MSG_FLAG_NEW for new messages, ie the ones that live in
 * new/. The flags are logically OR'ed. Note that the file does not
 * have to exist; the flags are based on the path only.
 *
 * @param pathname of a mailfile; it does not have to refer to an
 * actual message
 * 
 * @return the flags, or MU_MSG_FILE_FLAG_UNKNOWN in case of error
 */
MuMsgFlags mu_maildir_get_flags_from_path (const char* pathname);

/**
 * get the new pathname for a message, based on the old path and the
 * new flags. Note that setting/removing the MU_MSG_FLAG_NEW will
 * change the directory in which a message lives. The flags are as
 * specified in http://cr.yp.to/proto/maildir.html, plus
 * MU_MSG_FLAG_NEW for new messages, ie the ones that live in
 * new/. The flags are logically OR'ed. Note that the file does not
 * have to exist; the flags are based on the path only.
 * 
 * @param oldpath the old (current) full path to the message
 * (including the filename) *
 * @param newflags the new flags for this message
 * 
 * @return a new path name; use g_free when done with. NULL in case of
 * error.
 */
char* mu_maildir_get_path_from_flags (const char *oldpath,
				       MuMsgFlags newflags);

G_END_DECLS

#endif /*__MU_MAILDIR_H__*/