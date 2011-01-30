<?php
// $Id$
if (module_exists('taxonomy')) {
  $plugin = array(
    'form'     => 'feeds_tamper_taxonomy_keyword_filter_form',
    'callback' => 'feeds_tamper_taxonomy_keyword_filter_callback',
    'validate' => 'feeds_tamper_taxonomy_keyword_filter_validate',
    'name'     => 'Keyword Filter by Vocabulary',
    'multi'    => 'loop',
    'category' => 'Filter',
  );
}

function feeds_tamper_taxonomy_keyword_filter_form($importer, $element_key, $settings) {
  $form = array();

  $tax = taxonomy_get_vocabularies();
  $tax_options = array();

  foreach ($tax as $t) {
    $tax_options[$t->vid] = $t->name;
  }

  $form['t'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );
  $form['t']['vocabularies'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Vocabularies to use'),
    '#description' => t('Words from these vocabularies will be used to filter against.'),
    '#options' => $tax_options,
    '#default_value' => isset($settings['t']['vocabularies']) ? $settings['t']['vocabularies'] : array(),
  );
  $form['synonyms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use synonyms'),
    '#default_value' => isset($settings['synonyms']) ? $settings['synonyms'] : TRUE,
    '#description' => t('If checked, then synonyms of a term will be added.'),
  );
  $form['word_boundaries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Respect word boundaries'),
    '#default_value' => isset($settings['word_boundaries']) ? $settings['word_boundaries'] : TRUE,
    '#description' => t('If checked, then "book" will match "book" but not "bookcase".'),
  );
  $form['case_sensitive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Case sensitive'),
    '#default_value' => isset($settings['case_sensitive']) ? $settings['case_sensitive'] : FALSE,
    '#description' => t('If checked, then "book" will match "book" but not "Book" or "BOOK".')
  );
  return $form;
}

function feeds_tamper_taxonomy_keyword_filter_validate(&$settings) {
  $settings['regex'] = FALSE;

  if (!$settings['word_boundaries'] && $settings['case_sensitive']) {
    $settings['func'] = 'strpos';
  }
  else if (!$settings['word_boundaries'] && !$settings['case_sensitive']) {
    $settings['func'] = 'stripos';
  }
  else {
    $settings['regex'] = TRUE;
  }
}


function feeds_tamper_taxonomy_keyword_filter_callback($result, $item_key, $element_key, &$field, $settings) {
  static $words = array();
  if (!isset($words[$element_key])) {
    $words[$element_key] = array();
    foreach ($settings['t']['vocabularies'] as $vid) {
      foreach (taxonomy_get_tree($vid) as $term) {
        $word_array = array();
        if ($settings['synonyms']) {
          $word_array = taxonomy_get_synonyms($term->tid);
        }
        $word_array[] = $term->name;
        foreach ($word_array as $word) {
          if ($settings['regex']) {
            $word = '/\b' . $word . '\b/';
            if (!$settings['case_sensitive']) {
              $word .= 'i';
            }
          }
          $words[$element_key][] = $word;
        }
      }
    }
  }
  if ($settings['regex']) {
    foreach ($words[$element_key] as $word) {
      if (preg_match($word, $field) > 0) {
        return;
      }
    }
  }
  else {
    foreach ($words[$element_key] as $word) {
      if ($settings['func']($field, $word) !== FALSE) {
        return;
      }
    }
  }
  unset($result->items[$item_key]);
}
