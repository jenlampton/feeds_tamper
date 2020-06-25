<?php

/**
 * @file
 * Feeds plugins declarations.
 * 
 * Testing the calling of all plugins. Should we have one list here or call each individual file?
 */

/**
 * Implements _feeds_tamper_feeds_tamper_plugins().
 */
function _feeds_tamper_feeds_tamper_plugins() {
  $path = 'plugins/';
  $module = 'feeds_tamper';

  $info = array();
  $info['absolute_url'] = array(
    'form' => 'feeds_tamper_absolute_url_form',
    'callback' => 'feeds_tamper_absolute_url_callback',
    'name' => 'Make URLs absolute',
    'category' => 'HTML',
    'handler' => array(
      'file_type' => 'inc',
      'file' => 'absolute_url',
      'path' => $path,
      'module' => $module,
    ),
  );
  $info['array_filter'] = array(
    'form' => 'feeds_tamper_array_filter_form',
    'callback' => 'feeds_tamper_array_filter_callback',
    'name' => 'Filter empty items',
    'multi' => 'direct',
    'category' => 'List',
    'handler' => array(
      'file_type' => 'inc',
      'file' => 'array_filter',
      'path' => $path,
      'module' => $module,
    ),
  );
  $info['convert_case'] = array(
    'form' => 'feeds_tamper_convert_case_form',
    'callback' => 'feeds_tamper_convert_case_callback',
    'name' => 'Convert case',
    'multi' => 'loop',
    'category' => 'Text',
    'handler' => array(
      'file_type' => 'inc',
      'file' => 'convert_case',
      'path' => $path,
      'module' => $module,
    ),
  );
  $info['trim'] = array(
    'form' => 'feeds_tamper_trim_form',
    'callback' => 'feeds_tamper_trim_callback',
    'name' => 'Trim',
    'multi' => 'loop',
    'category' => 'Text',
    'handler' => array(
      'file_type' => 'inc',
      'file' => 'trim',
      'path' => $path,
      'module' => $module,
    ),
  );

  return $info;
}
