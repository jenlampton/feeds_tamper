<?php

/**
 * @file
 * Rewrite a field using tokens.
 */

$plugin = array(
  'form'     => 'feeds_tamper_rewrite_form',
  'callback' => 'feeds_tamper_rewrite_callback',
  'name'     => 'Rewrite',
  'multi'    => 'skip',
  'category' => 'Other',
);

function feeds_tamper_rewrite_form($importer, $element_key, $settings) {
  $form = array();
  // The CSV parser automagically lowercases all sources.
  $is_csv = FALSE;
  if (get_class($importer->parser) == 'FeedsCSVParser') {
    $is_csv = TRUE;
  }
  $mappings = $importer->processor->config['mappings'];
  $replace = array();

  foreach ($mappings as $mapping) {
    if ($is_csv && strpos($mapping['source'], 'Blank source ') !== 0) {
      $replace[] = '[' . drupal_strtolower($mapping['source']) . ']';
    }
    else {
      $replace[] = '[' . $mapping['source'] . ']';
    }
  }

  $form['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Replacement pattern'),
    '#default_value' => isset($settings['text']) ? $settings['text'] : '',
  );
  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available Replacement Patterns'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#value' => theme('item_list', array('items' => $replace)),
  );
  return $form;
}

function feeds_tamper_rewrite_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  $trans = array();

  static $sources;
  if (!$sources) {
    $sources = _feeds_tamper_rewrite_get_source_list($source);
  }

  $item = $result->items[$item_key];
  foreach ($sources as $source) {
    if (isset($item[$source])) {
      $trans["[$source]"] = $item[$source];
    }
    else {
      $trans["[$source]"] = '';
    }
  }
  $field = strtr($settings['text'], $trans);
}

function _feeds_tamper_rewrite_get_source_list($source) {
  $is_csv = FALSE;
  if (get_class($source->importer->parser) == 'FeedsCSVParser') {
    $is_csv = TRUE;
  }

  $sources = array();
  foreach ($source->importer->processor->getMappings() as $mapping) {
    if ($is_csv) {
      $sources[] = drupal_strtolower($mapping['source']);
    }
    else {
      $sources[] = $mapping['source'];
    }
  }

  return array_unique($sources);
}
