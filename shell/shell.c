/**
 * Copyright (C) 2023 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "shell.h"
#include "../containers/containers.h"
#include "../eval.h"
#include "../posix/posix.h"
#include "../posix/signal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef CHEZ_SCHEME_DIR
#error "please #define CHEZ_SCHEME_DIR to the installation path of Chez Scheme"
#endif

#define STR_(arg) #arg
#define STR(arg) STR_(arg)
#define CHEZ_SCHEME_DIR_STR STR(CHEZ_SCHEME_DIR)

/**
 * return i-th environment variable i.e. environ[i]
 * converted to a cons containing two Scheme strings: (key . value)
 *
 * if environ[i] is NULL, return #f
 */
static ptr c_environ_ref(uptr i) {
  const char* entry = environ[i];
  if (entry) {
    const char* separator = strchr(entry, '=');
    size_t      namelen   = separator ? separator - entry : 0;
    iptr        inamelen  = Sfixnum_value(Sfixnum(namelen));
    if (namelen > 0 && inamelen > 0 && namelen == (size_t)inamelen) {
      return Scons(Sstring_utf8(entry, inamelen), Sstring_utf8(separator + 1, -1));
    }
  }
  return Sfalse;
}

/**
 * return current working directory
 */
static ptr c_get_cwd(void) {
  {
    char dir[256];
    if (getcwd(dir, sizeof(dir)) == dir) {
      return Sstring_utf8(dir, -1);
    } else if (c_errno() != -ERANGE) {
      return Sstring_utf8("", 0);
    }
  }
  {
    size_t maxlen = 1024;
    char*  dir    = NULL;
    while (maxlen && (dir = malloc(maxlen)) != NULL) {
      if (getcwd(dir, maxlen) == dir) {
        ptr ret = Sstring_utf8(dir, -1);
        free(dir);
        return ret;
      }
      free(dir);
      maxlen *= 2;
    }
  }
  return Sstring_utf8("", 0);
}

int schemesh_register_c_functions(void) {
  int err;

  schemesh_register_c_functions_containers();

  if ((err = schemesh_register_c_functions_fd()) < 0) {
    return err;
  }
  schemesh_register_c_functions_signals();
  schemesh_register_c_functions_tty();
  schemesh_register_c_functions_posix();
  schemesh_register_c_functions_pid();

  Sregister_symbol("c_environ_ref", &c_environ_ref);
  Sregister_symbol("c_get_cwd", &c_get_cwd);

  return err;
}

void schemesh_compile_and_load_libraries(void) {
  eval("(let ((try-load\n"
       "  (lambda (path)\n"
       "    (call/cc\n"
       "      (lambda (k-exit)\n"
       "        (with-exception-handler\n"
       "          (lambda (cond)\n"
       "            (k-exit #f))\n"
       "          (lambda ()\n"
       "            (load path)\n"
       "            #t)))))))\n"
       "  (unless (try-load \"/usr/local/lib/libschemesh.so\")\n"
       "    (unless (try-load \"/usr/lib/libschemesh.so\")\n"
       "      (unless (try-load \"libschemesh.so\")\n"
       "        (compile-file \"libschemesh.ss\" \"libschemesh.debug.so\")\n"
       "        (strip-fasl-file \"libschemesh.debug.so\" \"libschemesh.so\"\n"
       "          (fasl-strip-options inspector-source source-annotations profile-source))\n"
       "        (load \"libschemesh.so\")))))\n");
}

void schemesh_import_libraries(void) {
  eval("(begin\n"
       "  (import (schemesh bootstrap))\n"
       "  (import (schemesh containers))\n"
       "  (import (schemesh conversions))\n"
       "  (import (schemesh io))\n"
       "  (import (schemesh parser))\n"
       "  (import (schemesh posix))\n"
       "  (import (schemesh lineedit base))\n"
       "  (import (schemesh lineedit))\n"
       "  (import (schemesh shell))\n"
       "  (import (schemesh repl)))\n");
}

void schemesh_init(void (*on_scheme_exception)(void)) {
  Sscheme_init(on_scheme_exception);
  Sregister_boot_file(CHEZ_SCHEME_DIR_STR "/petite.boot");
  Sregister_boot_file(CHEZ_SCHEME_DIR_STR "/scheme.boot");
  Sbuild_heap(NULL, NULL);
}

void schemesh_quit(void) {
  Sscheme_deinit();
}
