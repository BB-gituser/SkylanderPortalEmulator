#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"

#include "hw.h"

#include "ff.h"

#include "../includes/sd.h"

#define SCAN_MAX_DEPTH 8
#define SCAN_SKIP_HIDDEN 1

#define NAME_SIZE (sizeof(((File *)0)->filename))
#define PATH_SIZE (sizeof(((File *)0)->path))

int nfile = 0;

static void copy_str(char *dst, size_t dst_size, const char *src) {
  size_t n = strlen(src);
  if (n >= dst_size)
    n = dst_size - 1;
  memcpy(dst, src, n);
  dst[n] = '\0';
}

static size_t joined_len(const char *base, const char *name) {
  size_t blen = strlen(base);
  bool need_slash = (blen > 0 && base[blen - 1] != '/');
  return blen + (need_slash ? 1 : 0) + strlen(name);
}

static void join_path(char *dst, size_t dst_size, const char *base,
                      const char *name) {
  size_t blen = strlen(base);
  if (blen >= dst_size)
    blen = dst_size - 1;
  memcpy(dst, base, blen);
  if (blen > 0 && dst[blen - 1] != '/' && blen + 1 < dst_size)
    dst[blen++] = '/';
  copy_str(dst + blen, dst_size - blen, name);
}

static void set_folder_name(Folder *f) {
  size_t len = strlen(f->path);
  while (len > 1 && f->path[len - 1] == '/')
    len--;
  size_t start = len;
  while (start > 0 && f->path[start - 1] != '/')
    start--;

  size_t n = len - start;
  if (n >= sizeof f->foldername)
    n = sizeof f->foldername - 1;
  memcpy(f->foldername, f->path + start, n);
  f->foldername[n] = '\0';
}

static bool keep_entry(const FILINFO *fno, const char *base) {
#if SCAN_SKIP_HIDDEN
  if (fno->fattrib & (AM_HID | AM_SYS))
    return false;
#endif
  return joined_len(base, fno->fname) < PATH_SIZE;
}

static bool scan_into(Folder *self, int depth) {
  self->file_count = 0;
  self->folder_count = 0;
  self->folderlist = NULL;
  self->filelist = NULL;

  DIR *dir = malloc(sizeof *dir);
  FILINFO *fno = malloc(sizeof *fno);
  bool ok = false;

  if (!dir || !fno)
    goto end;
  if (f_opendir(dir, self->path) != FR_OK)
    goto end;

  int nfiles = 0, nfolders = 0;
  while (f_readdir(dir, fno) == FR_OK && fno->fname[0] != '\0') {
    if (!keep_entry(fno, self->path))
      continue;
    if (fno->fattrib & AM_DIR)
      nfolders++;
    else
      nfiles++;
  }

  if (nfolders > 0)
    self->folderlist = calloc((size_t)nfolders, sizeof(Folder));
  if (nfiles > 0)
    self->filelist = calloc((size_t)nfiles, sizeof(File));
  if ((nfolders > 0 && !self->folderlist) || (nfiles > 0 && !self->filelist)) {
    f_closedir(dir);
    free(self->folderlist);
    self->folderlist = NULL;
    free(self->filelist);
    self->filelist = NULL;
    goto end;
  }

  f_rewinddir(dir);
  int fi = 0, di = 0;
  while (f_readdir(dir, fno) == FR_OK && fno->fname[0] != '\0') {
    if (!keep_entry(fno, self->path))
      continue;

    if (fno->fattrib & AM_DIR) {
      if (di >= nfolders)
        continue;
      Folder *child = &self->folderlist[di++];
      copy_str(child->foldername, sizeof child->foldername, fno->fname);
      join_path(child->path, sizeof child->path, self->path, fno->fname);
      child->parent = self;
    } else {
      if (fi >= nfiles)
        continue;
      File *file = &self->filelist[fi++];
      copy_str(file->filename, NAME_SIZE, fno->fname);
      file->FileIndex = nfile;
      nfile++;
      join_path(file->path, PATH_SIZE, self->path, fno->fname);
      file->parent = self;
    }
  }
  f_closedir(dir);

  self->folder_count = di;
  self->file_count = fi;
  ok = true;

end:
  free(fno);
  free(dir);

  if (ok && depth + 1 < SCAN_MAX_DEPTH) {
    for (int i = 0; i < self->folder_count; i++)
      scan_into(&self->folderlist[i], depth + 1);
  }
  return ok;
}

Folder *scan_directory(char path[256], Folder *parent_folder) {
  Folder *folder = calloc(1, sizeof *folder);
  if (!folder)
    return NULL;

  copy_str(folder->path, sizeof folder->path, path);
  set_folder_name(folder);
  folder->parent = parent_folder;

  if (!scan_into(folder, 0)) {
    free(folder);
    return NULL;
  }
  return folder;
}

Folder *scan_sd_root(void) {
  char root_path[256] = "0:/Skylanders/";
  return scan_directory(root_path, NULL);
}
