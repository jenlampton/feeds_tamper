<?php

/**
 * @file
 * Feeds related api hooks.
 */

/**
 * Implements hook_feeds_after_parse().
 *
 * This is the meat of the whole deal.  After every Feeds run, before going into
 * processing, this gets called and modifies the data based on the configuration.
 */
function feeds_tamper_feeds_after_parse(FeedsSource $source, FeedsParserResult $result) {
  $importer_instances = feeds_tamper_load_by_importer($source->importer->id, FALSE, TRUE);
  if (empty($importer_instances)) {
    return;
  }
  foreach ($result->items as $item_key => &$item) {
    foreach ($importer_instances as $element_key => $instances) {
      foreach ($instances as $instance) {
        $plugin = feeds_tamper_get_plugin($instance->plugin_id);
        $is_array = is_array($item[$element_key]);

        if ($is_array && $plugin['multi'] == 'loop') {
          foreach ($item[$element_key] as &$i) {
            $plugin['callback']($result, $item_key, $element_key, $i, $instance->settings);
          }
        }
        elseif ($is_array && $plugin['multi'] == 'direct') {
          $plugin['callback']($result, $item_key, $element_key, $item[$element_key], $instance->settings);
        }
        elseif (!$is_array && $plugin['single'] != 'skip') {
          $plugin['callback']($result, $item_key, $element_key, $item[$element_key], $instance->settings);
        }
      }
    }
  }
}

///**
// * Implements hook_feeds_processor_targets_alter().
// */
//function feeds_tamper_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
//  dpm($targets);
//  $targets['feeds_tamper_store'] = array(
//    'name' => t('Temporary target'),
//    'description' => t('A field that stores the source data temporarily so that it can be used with the Feeds Tamper rewrite plugin.'),
//    'callback' => 'feeds_tamper_store',
//  );
//}
//
///**
// * Implements hook_feeds_parser_sources_alter().
// */
//function feeds_tamper_feeds_parser_sources_alter(&$sources, $content_type) {
//  $sources['feeds_tamper_blank'] = array(
//    'name' => t('Blank source'),
//    'description' => t("A source provided by Feeds Tamper with no value of it's own."),
//  );
//}
