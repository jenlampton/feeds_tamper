<?php

/**
 * @file
 * Version agnostic parts of feeds_tamper.module.
 */


/**
 * @defgroup feeds_tamper_api Feeds Tamper API
 * @{
 * API functions for dealing with plugins and plugin instances.
 */

/**
 * Load all plugin instances.
 *
 * @param bool $disabled
 *   (optional) If TRUE, load disabled plugin instances. Defaults to FALSE.
 *
 * @return array
 *   An associative array of plugin instances keyed to their id.
 */
function feeds_tamper_load_all_instances($disabled = FALSE) {
  $instances = array();

  $configs = feeds_tamper_load_tamper_object();
  foreach ($configs as $instance_id => $config) {
    if (!empty($config->id) && ($disabled || empty($config->disabled))) {
      $instances[$instance_id] = $config;
    }
  }
  return $instances;
}

/**
 * Load a plugin instance by id.
 *
 * @param string $id
 *   The id of the plugin instance.
 *
 * @return stdClass
 *   A plugin instance object.
 */
function feeds_tamper_load_instance($id) {
  $id = backdrop_substr($id, 0, 127);
  $source = feeds_tamper_get_source_from_instance($id);
  $instance = $source->get('instances.' . $id);
  $instance = !empty($instance) ? (object) $instance : NULL;

  return $instance;
}

/**
 * Find the source and importer from the instance id.
 * 
 * @param string $id
 *   The id of the plugin instance.
 * 
 * @return Config
 *   The source config object.
 */
function feeds_tamper_get_source_from_instance($id) {
  $chunks = explode('-', $id);
  return config('feeds_tamper.' . $chunks[0] . '.' . $chunks[1]);
}

/**
 * Save a plugin instance.
 *
 * @param stdClass $instance
 *   A plugin instance object.
 *
 * @return mixed
 *   If the plugin save failed, returns FALSE. If it succeeded, returns
 *   SAVED_NEW or SAVED_UPDATED, depending on the operation performed.
 */
function feeds_tamper_save_instance($instance) {
  $instance->id = backdrop_substr($instance->id, 0, 127);

  $prefix = 'feeds_tamper.' . $instance->importer . '.' . $instance->source;
  $source = config($prefix);

  // It's a new instance, give it the heaviest weight.
  if (!isset($instance->weight)) {
    $weight = 0;
    foreach ($source->get('instances') as $i) {
      if ($i['weight'] >= $weight) {
        $weight = $i['weight'] + 1;
      }
    }
    $instance->weight = $weight;
  }

  if (!isset($instance->settings)) {
    $instance->settings = array();
  }

  $instance->disabled = !empty($instance->disabled) ? TRUE : FALSE;

  $source->set('importer', $instance->importer);
  $source->set('source', $instance->source);
  $source->set('instances.' . $instance->id, $instance);
  $source->save();
}

/**
 * Removes plugin instances whose source was removed.
 *
 * @param FeedsImporter $importer
 *   The importer to rectify against.
 */
function feeds_tamper_rectify_instances(FeedsImporter $importer) {
  $instances = feeds_tamper_load_by_importer($importer);
  $sources = feeds_tamper_get_unique_source_list($importer, FALSE);

  foreach ($instances as $instance) {
    if (!is_object($instance)) {
      continue;
    }
    if (in_array($instance->source, $sources)) {
      continue;
    }

    feeds_tamper_delete_instance($instance);
  }
}

/**
 * Delete a single plugin instance.
 *
 * @param string|object $instance
 *   A plugin instance object or the id of a plugin instance.
 */
function feeds_tamper_delete_instance($instance) {
  // Allow for string id or plugin object.
  if (is_scalar($instance)) {
    $instance = feeds_tamper_load_instance($instance);
  }

  $source = feeds_tamper_get_source_from_instance($instance->id);
  $source->clear('instances.' . $instance->id);
  $source->save();
}

/**
 * Load plugin instances by importer id.
 *
 * @param string|FeedsImporter $importer
 *   The importer id, or object to reference.
 * @param bool $disabled
 *   (optional) If TRUE load disabled plugin instances. Defaults to FALSE.
 *
 * @return array
 *   An associative array of plugin instances, keyed by source.
 */
function feeds_tamper_load_by_importer($importer, $disabled = FALSE) {
  if (is_scalar($importer)) {
    $importer = feeds_importer($importer);
  }

  $sources = config_get_names_with_prefix('feeds_tamper.' . $importer->id);

  $return = array();
  foreach ($sources as $i) {
    $source_config = config($i);
    $source = $source_config->get('source');
    $instances = $source_config->get('instances');
    foreach ($instances as &$instance) {
      $instance = (object) $instance;
    }
    $return[$source] = $instances;
  }
  // foreach ($instances as &$instance_list) {
  //   usort($instance_list, '_feeds_tamper_cmp');
  // }

  // Sort the plugins by the order they appear on the mapping page.
  // foreach ($sources as $source) {
  //   if (isset($instances[$source])) {
  //     $return[$source] = $instances[$source];
  //   }
  // }

  return $return;
}

/**
 * Returns a unique list of sources in order.
 *
 * @param FeedsImporter $importer
 *   The importer.
 *
 * @return array
 *   A list of sources.
 */
function feeds_tamper_get_unique_source_list($importer, $lower = TRUE) {
  $sources = array();
  $is_csv = $lower && ($importer->parser instanceof FeedsCSVParser);

  foreach ($importer->processor->getMappings() as $mapping) {
    if ($is_csv) {
      $sources[] = backdrop_strtolower($mapping['source']);
    }
    else {
      $sources[] = $mapping['source'];
    }
  }

  return array_unique($sources);
}

/**
 * Comparison callback that sorts by weight, then alphabetically by id.
 */
function _feeds_tamper_cmp($a, $b) {
  // If weights are equal compare id's.
  if ($a->weight == $b->weight) {
    $tmp = array($a->id, $b->id);
    sort($tmp);
    if ($tmp[0] == $a->id) {
      return -1;
    }
    return 1;
  }
  return ($a->weight < $b->weight) ? -1 : 1;
}

/**
 * Get all available plugins.
 *
 * @return array
 *   An associative array where the keys are the plugin keys and the values are
 *   the plugin info arrays as defined in a plugin include file.
 */
function feeds_tamper_get_plugins($plugin = NULL) {
  $plugins = module_invoke_all('feeds_tamper_plugins');

  return $plugins;
}

/**
 * Get a single plugin.
 *
 * @param string $id
 *   The id of a plugin.
 *
 * @return stdClass
 *   A plugin object.
 */
function feeds_tamper_get_plugin($id) {
  $plugins = module_invoke_all('feeds_tamper_plugins');

  return $plugins[$id];
}

/**
 * Return a machine name safe version of a string.
 *
 * @param string $string
 *   String to get machine nameized.
 *
 * @return string
 *   A lowercase string with all values not in [a-zA-Z0-9] replaced with an
 *   underscore and shortened to 128 characters.
 */
function feeds_tamper_make_machine($string) {
  return backdrop_substr(preg_replace('/[^a-z0-9-]/u', '_', backdrop_strtolower($string)), 0, 127);
}

/**
 * @} End of "feeds_tamper_api".
 */

/**
 * Menu access callback.
 *
 * @param string|FeedsImporter $importer
 *   The importer or importer id being tampered with.
 * @param string|stdClass $instance
 *   (optional) If set, the importer attached to $instance will be tried first.
 *   Defaults to NULL.
 *
 * @return bool
 *   TRUE if the user has access, FALSE if not.
 */
function feeds_tamper_access($importer, $instance = NULL) {
  if (isset($instance)) {
    if (is_object($instance)) {
      $importer = $instance->importer;
    }
    else {
      $importer = feeds_tamper_load_instance($instance)->importer;
    }
  }
  elseif (is_object($importer)) {
    $importer = $importer->id;
  }

  // Verify actual input if above failed.
  if ($importer) {
    // Check for permissions, otherwise return FALSE.
    if (user_access('administer feeds_tamper') || user_access('tamper ' . $importer)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Implements hook_form_feeds_ui_mapping_form_alter().
 *
 * This is an interesting bit of work. Each source name has to be unique,
 * but we have no idea how many to create with
 * feeds_tamper_feeds_parser_sources_alter() because we don't know how many
 * targets there are going to be.
 *
 * The solution is to keep track in the form how many have been added.
 */
function feeds_tamper_form_feeds_ui_mapping_form_alter(array &$form, array &$form_state) {
  $form['#submit'][] = 'feeds_tamper_form_feeds_ui_mapping_form_submit';

  // Don't alter the sources on the form for parsers that use manual input.
  $manual_source_input = ($form['source']['#type'] == 'textfield');

  $max_source = 0;
  $max_target = 0;
  foreach ($form['#mappings'] as $mapping) {
    if (!$manual_source_input && strpos($mapping['source'], 'Blank source ') === 0) {
      list(, , $source_value) = explode(' ', $mapping['source']);
      if ($source_value > $max_source) {
        $max_source = $source_value;
      }
    }
    if (strpos($mapping['target'], 'Temporary target ') === 0) {
      list(, , $target_value) = explode(' ', $mapping['target']);
      if ($target_value > $max_target) {
        $max_target = $target_value;
      }
    }
  }

  if (!$manual_source_input) {
    if ($max_source) {
      unset($form['source']['#options']['Blank source 1']);
      $form['source']['#options']['Blank source ' . ++$max_source] = 'Blank source';

      // Declare extra mapping sources.
      for ($i = 2; $i <= $max_source; $i++) {
        $form['#feeds_sources']['Blank source ' . $i] = $form['#feeds_sources']['Blank source 1'];
      }
    }
    else {
      $form['source']['#options']['Blank source 1'] = 'Blank source';
    }
  }

  if ($max_target) {
    unset($form['target']['#options']['Temporary target 1']);
    $form['target']['#options']['Temporary target ' . ++$max_target] = 'Temporary target';

    // Declare extra mapping targets.
    for ($i = 2; $i <= $max_target; $i++) {
      $form['#feeds_targets']['Temporary target ' . $i] = $form['#feeds_targets']['Temporary target 1'];
    }
  }
  else {
    $form['target']['#options']['Temporary target 1'] = 'Temporary target';
  }
}

/**
 * Submit callback for feeds_ui_mapping_form().
 */
function feeds_tamper_form_feeds_ui_mapping_form_submit(array &$form, array &$form_state) {
  feeds_tamper_rectify_instances(feeds_importer($form['#importer']));
}
