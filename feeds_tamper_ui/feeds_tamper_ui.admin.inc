<?php

/**
 * @file
 * Forms and their accompanying validation and submit functions for Feeds Tamper
 * UI.
 */

function feeds_tamper_ui_list_form($form, &$form_state, $importer) {
  $mappings = $importer->processor->config['mappings'];
  $instances = feeds_tamper_load_by_importer($importer->id, TRUE);
  $form['mappings'] = array();
  $form['#importer'] = $importer;

  foreach ($mappings as $mapping) {
    $form['mappings'][$mapping['source']] = array(
      '#title' => check_plain($mapping['source']) . ' -> ' . check_plain($mapping['target']),
      '#tree' => TRUE,
    );
    $form['mappings'][$mapping['source']]['table'] = array();

    if (!isset($instances[$mapping['source']])) {
      $instances[$mapping['source']] = array();
    }
    foreach ($instances[$mapping['source']] as $instance) {
      $plugin = feeds_tamper_get_plugin($instance->plugin_id);
      if ($instance->export_type == EXPORT_IN_CODE) {
        $status = t('Default');
        $edit = t('Override');
        $delete = '';
      }
      elseif ($instance->export_type == EXPORT_IN_DATABASE) {
        $status = t('Normal');
        $edit = t('Edit');
        $delete = t('Delete');
      }
      elseif ($instance->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
        $status = t('Overridden');
        $edit = t('Edit');
        $delete = t('Revert');
      }
      $instance->disabled = !isset($instance->disabled) ? FALSE : $instance->disabled;
      $checkbox = array('#type' => 'checkbox', '#default_value' => !$instance->disabled);
      $form['mappings'][$mapping['source']]['table'][$instance->id]['enabled'] = $checkbox;
      $form['mappings'][$mapping['source']]['table'][$instance->id]['description']['#value'] = !empty($instance->description) ? check_plain($instance->description) : $instance->id;
      $form['mappings'][$mapping['source']]['table'][$instance->id]['name']['#value'] = $plugin['name'];
      $form['mappings'][$mapping['source']]['table'][$instance->id]['status']['#value'] = $status;
      $form['mappings'][$mapping['source']]['table'][$instance->id]['edit']['#value'] =
        l(t($edit), 'admin/structure/feeds/tamper/edit/' . $instance->id) . (empty($delete) ? '' :  ' | ' . l(t($delete), 'admin/structure/feeds/tamper/delete/' . $instance->id));
      $form['mappings'][$mapping['source']]['table'][$instance->id]['weight-' . $instance->id] = array(
        '#type' => 'textfield',
        '#size' => 5,
        '#default_value' => $instance->weight,
        '#attributes' => array('class' => array('weight')),
      );
    }
    $add = l(t('Add plugin'), 'admin/structure/feeds/tamper/add/' . $importer->id . '/' . $mapping['source']);
    $form['mappings'][$mapping['source']]['add_link']['#value'] = '<ul class="feeds-tamper-add action-links"><li>' . $add . '</li></ul>';
  }
  $form['help']['#markup'] = '<div class="help"><p>' . t('Configure plugins to modify Feeds data before it gets saved. Each !mapping can be manipulated individually.', array('!mapping' => l(t('mapping'), 'admin/structure/feeds/edit/' . $importer->id . '/mapping'))) . '</p></div>';

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );
  return $form;
}

function feeds_tamper_ui_list_form_submit($form, &$form_state) {
  $mappings = element_children($form['mappings']);
  $disabled = array();
  foreach ($mappings as $mapping) {
    if (isset($form_state['values'][$mapping])) {
      foreach ($form_state['values'][$mapping]['table'] as $id => $value) {
        $instance = feeds_tamper_load_instance($id);
        $weight = (int) $value['weight-' . $id];
        $disabled[$id] = !$value['enabled'];
        if ($instance->weight != $weight) {
          $instance->weight = $weight;
          ctools_export_crud_save('feeds_tamper', $instance);
        }
      }
    }
  }
  variable_set('default_feeds_tamper', $disabled);
}

function theme_feeds_tamper_ui_list_form($variables) {
  $form = $variables['form'];
  $output = '';
  $mappings = element_children($form['mappings']);

  if (empty($mappings)) {
    // Can't use drupal_set_message here.
    $output .= '<div class="messages warning">';
    $output .= t('There are no !mappings defined for this importer.', array('!mappings' => l(t('mappings'), 'admin/structure/feeds/edit/' . $form['#importer']->id . '/mapping')));
    $output .= '</div>';
    return $output;
  }
  foreach ($mappings as $source) {
    $table_rows = $disabled_rows = array();
    $table_id = feeds_tamper_make_machine($source) . '-table';
    $header = array(t('Description'), t('Weight'), t('Plugin'), t('Status'), t('Operations'), t('Enabled'));
    $children = element_children($form['mappings'][$source]['table']);

    if (empty($children)) {
      $help_text = t('No plugins defined.');
      $table_rows = array('data' => array(array('data' => $help_text, 'colspan' => 5)));
      $header = array(t('Description'), t('Plugin'), t('Status'), t('Operations'), t('Enabled'));
    }
    else {
      foreach ($children as $id) {
        $disabled = empty($form['mappings'][$source]['table'][$id]['enabled']['#default_value']);
        if (!$disabled) {
          $this_row = array();
          $this_row[] = $form['mappings'][$source]['table'][$id]['description']['#value'];
          $this_row[] = drupal_render($form['mappings'][$source]['table'][$id]['weight-' . $id]);
          $this_row[] = $form['mappings'][$source]['table'][$id]['name']['#value'];
          $this_row[] = $form['mappings'][$source]['table'][$id]['status']['#value'];
          $this_row[] = $form['mappings'][$source]['table'][$id]['edit']['#value'];
          $this_row[] = drupal_render($form['mappings'][$source]['table'][$id]['enabled']);
          $table_rows[] = array('data' => $this_row, 'class' => array('draggable'));
        }
        else {
          $this_row = array();
          $this_row[] = $form['mappings'][$source]['table'][$id]['description']['#value'];
          $this_row[] = drupal_render($form['mappings'][$source]['table'][$id]['weight-' . $id]);
          $this_row[] = $form['mappings'][$source]['table'][$id]['name']['#value'];
          $this_row[] = $form['mappings'][$source]['table'][$id]['status']['#value'];
          $this_row[] = strip_tags($form['mappings'][$source]['table'][$id]['edit']['#value']);
          $this_row[] = drupal_render($form['mappings'][$source]['table'][$id]['enabled']);
          $disabled_rows[] = array('data' => $this_row, 'class' => array('disabled'));
        }
      }
      drupal_add_tabledrag($table_id, 'order', 'sibling', 'weight');
    }
    $table_rows = array_merge($table_rows, $disabled_rows);
    $add = $form['mappings'][$source]['add_link']['#value'];
    $table_rows[] = array('data' => array(array('data' => $add, 'colspan' => 7)), 'class' => array('feeds-tamper-add'));
    $table = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $table_rows,
      '#caption' => $form['mappings'][$source]['#title'],
      '#sticky' => FALSE,
    );
    $table['#attributes']['id'] = $table_id;
    //$table['#attributes']['class'][] = 'feeds-tamper-table';
    $table['#attached']['css'][] = drupal_get_path('module', 'feeds_tamper_ui') . '/feeds_tamper_ui.css';
    $form['mappings'][$source]['table'] = $table;
    $form['mappings'][$source]['#prefix'] = '<div class="feeds-tamper-table">';
    $form['mappings'][$source]['#suffix'] = '</div>';
  }
  $output .= $form['help']['#markup'];
  unset($form['help']);
  $output .= drupal_render_children($form);
  return $output;
}

function feeds_tamper_ui_add_plugin_form($form, &$form_state, $importer, $source) {
  // Set breadcrumb.
  $crumb = drupal_get_breadcrumb();
  $crumb[] = l(t('Tamper: @importer', array('@importer' => $importer->config['name'])), 'admin/structure/feeds/tamper/list/' . $importer->id);
  drupal_set_breadcrumb($crumb);

  // Set importer and source for use in validate and submit.
  $form_state['importer'] = $importer;
  $form_state['source'] = $source;

  // Build plugin select list.
  $feeds_tamper_plugins = feeds_tamper_get_plugins();
  $plugins = array();
  foreach ($feeds_tamper_plugins as $plugin_id => $plugin) {
    $plugins[t($plugin['category'])][$plugin_id] = t($plugin['name']);
  }
  ksort($plugins);
  foreach ($plugins as &$p) {
    asort($p);
  }

  $machine_name = key(reset($plugins));
  if (!empty($form_state['values']['plugin_id'])) {
    $machine_name = $form_state['values']['plugin_id'];
  }

  $plugin = feeds_tamper_get_plugin($machine_name);

  $form['plugin_id'] = array(
    '#title' => 'The plugin to add',
    '#type' => 'select',
    '#options' => $plugins,
    '#default_value' => '',
    '#tree' => TRUE,
    '#ajax' => array(
      'callback' => 'feeds_tamper_ajax_callback',
      'wrapper' => 'feeds-tamper-plugin',
    ),
    '#suffix' => '<br />',
  );
  $form['plugin']['#prefix'] = '<div class="clear-fix" id="feeds-tamper-plugin">';
  $form['plugin']['#suffix'] = '</div>';

  $form['plugin']['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $plugin['default description'] ? t($plugin['default description']) : t($plugin['name']),
    '#required' => TRUE,
    '#description' => t('A useful description of what this plugin is doing.'),
    '#tree' => TRUE,
  );
  $form['plugin']['id'] = array(
    '#title' => t('Machine name'),
    '#type' => 'machine_name',
    '#default_value' => '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'feeds_tamper_machine_name_callback',
      'source' => array('plugin', 'description'),
    ),
    '#disabled' => FALSE,
    '#tree' => TRUE,
  );
  $form['plugin']['settings'] = array(
    '#title' => t('Configure @name', array('@name' => $plugin['name'])),
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );
  $form['plugin']['settings'] += $plugin['form']($importer, $source, array());

  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  return $form;
}

function feeds_tamper_ui_add_plugin_form_validate($form, &$form_state) {
  // If ajax call, reset some $_POST values.
  if ($form_state['triggering_element']['#type'] == 'select') {
    unset($form_state['input']['description']);
    unset($form_state['input']['settings']);
    unset($form_state['input']['id']);
  }
  else {
    $plugin_id = $form_state['values']['plugin_id'];
    $plugin = feeds_tamper_get_plugin($plugin_id);
    if ($plugin['validate'] && isset($form_state['values']['settings'])) {
      $plugin['validate']($form_state['values']['settings']);
    }
    $form_state['values']['id'] =
      $form_state['importer']->id .
      '-' .
      feeds_tamper_make_machine($form_state['source']) .
      '-' .
      $form_state['values']['id'];
  }
}

function feeds_tamper_ui_add_plugin_form_submit($form, &$form_state) {
  $obj = new stdClass();
  $obj->plugin_id = $form_state['values']['plugin_id'];
  if (isset($form_state['values']['settings'])) {
    $obj->settings = $form_state['values']['settings'];
  }
  $obj->importer = $form_state['importer']->id;
  $obj->source = $form_state['source'];
  $obj->export_type = NULL;
  $obj->description = $form_state['values']['description'];
  $obj->id = $form_state['values']['id'];

  feeds_tamper_save_instance($obj);
  $form_state['redirect'] = 'admin/structure/feeds/tamper/list/' . $obj->importer;
}

function feeds_tamper_ui_edit_plugin_form($form, &$form_state, $instance) {
  // Set breadcrumb.
  $importer = feeds_importer($instance->importer);
  $crumb = drupal_get_breadcrumb();
  $crumb[] = l(t('Tamper: @importer', array('@importer' => $importer->config['name'])), 'admin/structure/feeds/tamper/list/' . $instance->importer);
  drupal_set_breadcrumb($crumb);

  $form_state['instance'] = $instance;
  $plugin = feeds_tamper_get_plugin($instance->plugin_id);
  $form['#tree'] = TRUE;

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#description' => t('A useful description of what this plugin is doing.'),
    '#default_value' => $instance->description,
  );
  $form['settings'] = array(
    '#title' => t('Configure @plugin', array('@plugin' => $plugin['name'])),
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );
  $form['settings'] += $plugin['form']($importer, $instance->source, $instance->settings);

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function feeds_tamper_ui_edit_plugin_form_validate($form, &$form_state) {
  $plugin_id = $form_state['instance']->plugin_id;
  $plugin = feeds_tamper_get_plugin($plugin_id);
  if ($plugin['validate']) {
    $plugin['validate']($form_state['values']['settings']);
  }
}

function feeds_tamper_ui_edit_plugin_form_submit($form, &$form_state) {
  $instance = $form_state['instance'];
  if (isset($form_state['values']['settings'])) {
    $instance->settings = $form_state['values']['settings'];
  }
  $instance->description = $form_state['values']['description'];

  feeds_tamper_save_instance($instance);
  $form_state['redirect'] = 'admin/structure/feeds/tamper/list/' . $instance->importer;
}

function feeds_tamper_ui_delete_form($form, &$form_state, $instance) {
  // Set breadcrumb.
  $importer = feeds_importer($instance->importer);
  $crumb = drupal_get_breadcrumb();
  $crumb[] = l(t('Tamper: @importer', array('@importer' => $importer->config['name'])), 'admin/structure/feeds/tamper/list/' . $instance->importer);
  drupal_set_breadcrumb($crumb);

  $form_state['instance'] = $instance;

  if ($instance->export_type & EXPORT_IN_CODE) {
    $question = t('Would you really like to revert the plugin @instance?', array('@instance' => $instance->description));
    $button_label = t('Revert');
  }
  else {
    $question = t('Would you really like to delete the plugin @instance?', array('@instance' => $instance->description));
    $button_label = t('Delete');
  }
  return confirm_form(
    $form,
    $question,
    'admin/structure/feeds/tamper/list/' . $instance->importer,
    NULL,
    $button_label
  );
}

function feeds_tamper_ui_delete_form_submit($form, &$form_state) {
  feeds_tamper_delete_instance($form_state['instance']);
  $form_state['redirect'] = 'admin/structure/feeds/tamper/list/' . $form_state['instance']->importer;
}

/**
 * Helper functions for ajax.
 */

function feeds_tamper_machine_name_callback($id, $form, $form_state) {
  $real_id = $form_state['importer']->id .
    '-' .
    feeds_tamper_make_machine($form_state['source']) .
    '-' .
    $id;
  return feeds_tamper_load_instance($real_id);
}

function feeds_tamper_ajax_callback($form, $form_state) {
  // The form has already been submitted and updated. We can return the replaced
  // item as it is.
  return $form['plugin'];
}
