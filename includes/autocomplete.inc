<?php
/**
 * @file
 * Autocomplete for the usc_mirc module.
 */

/**
 * Array_filter callback to find filenames which represent a video file.
 */
function usc_mirc_is_video_filename($filename) {
  static $exts = NULL;
  if (empty($exts)) {
    $exts = explode(' ', variable_get('usc_mirc_streaming_extensions', '.m4v .mp4 .flv .mov'));
  }
  return in_array(substr($filename, -4), $exts);
}

/**
 * Get an array of the files in this directory. Includes subdirs when $recurse.
 *
 * XXX:  This could be made a great deal more efficient in the case of a deep
 *  hierarchy, but anyway.
 */
function usc_mirc_traverse_directory($url_base, $filesystem_base, $string, $sub_dir = '.', $recurse = TRUE) {
  $videos = array();

  $dir_name = "$filesystem_base/$sub_dir";

  if ($sub_dir === '.') {
    $dir_name = $filesystem_base;
  }

  if (is_dir($dir_name)) {
    // XXX: dir() is detected as being an alias to a function which doesn't
    // exist...
    // @codingStandardsIgnoreStart
    $dir = dir($dir_name);
    // @codingStandardsIgnoreEnd
    while ($entry = $dir->read()) {
      if (in_array($entry, array('.', '..'))) {
        continue;
      }
      else {
        $this_entry = "$sub_dir/$entry";
        if ($sub_dir === '.') {
          $this_entry = $entry;
        }

        if ($recurse && is_dir("$filesystem_base/$this_entry")) {
          $videos = array_merge($videos, usc_mirc_traverse_directory($url_base, $filesystem_base, $string, $this_entry, $recurse));
        }
        // XXX:  Should probably get rid of the wildcard.
        elseif (($string === '*' || strpos($entry, $string) === 0)) {
          // If the search string doesn't start with '._' and the entry does,
          // avoid returning the given entry.
          if (strpos($string, '._') !== 0 && strpos($entry, '._') === 0) {
            continue;
          }
          $videos["$url_base/$this_entry"] = $entry;
        }
      }
    }
    $dir->close();
  }
  return $videos;
}

/**
 * Autocomplete functionality for selecting a preservation master directory.
 *
 * @param string $folder_name
 *   The value being searched for in the autocomplete.
 */
function usc_mirc_autocomplete_preservation_master($folder_name) {
  $folder_base = variable_get('usc_mirc_preservation_folder', '/mnt/mirc/arch/archival');
  $all_folders = glob("{$folder_base}/*", GLOB_ONLYDIR);

  $folders = array();
  foreach ($all_folders as $path) {
    if ($folder_name === '*' || strpos(basename($path), $folder_name) === 0) {
      $folders[$path] = basename($path);
    }
  }
  ksort($folders);
  drupal_json_output($folders);
}