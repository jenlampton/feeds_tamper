<?php
/**
 * @file
 * Sample hooks demonstrating how to create a custom plugin for Feeds Tamper.
 *
 * Feeds Tamper's hooks enable other modules to create their own tamper
 * plugins and register them with Feeds Tamper to be picked up automatically.
 *
 * Coming from Drupal? You will need to update your plugin to remove the ctools
 * code and add a 'filepath' entry directly in the plugin array.
 *
 */


/**
  * 
  * Add a file named my_plugin.inc to your custom module, containing the 
  * following. Start the file with `<?php` of course.
  *
  * $plugin keys
  * -------------
  *   'multi' =>    can be either 'direct' or 'loop'. This specifies how to 
  *                 handle multiple valued fields. 'direct' will pass the whole
  *                 array into the callback, whereas 'loop' will loop over the 
  *                 data, passing each value individually.
  *   'single' =>   'skip' can be set if your plugin only wants to handle 
  *                 multiple-valued fields directly. 
  *   'filepath' => full path to your includes file
  *
  * Validate
  * -------------
  * The validate callback is optional and can be used to set form errors on 
  * invalid input. There is, however, a more important task which the validate 
  * callback can be used for. It can be used to pre-compute values for the 
  * callback so that the callback has to do as little work as possible. See 
  * keyword_filter.inc for an example of this.
  *
  * Callback
  * -------------
  * The full Feeds item can be accessed through 
  * $feeds_parser_result->items[$item_key], with 
  * $feeds_parser_result->items[$item_key][$element_key] being the value of 
  * $field. This is provided to allow modifying/removing the Feeds item as a 
  * whole. See keyword_filter.inc for an example.
  *
  * The Feeds source ($feeds_source) can be used to access useful information, 
  * such as mapping settings, the raw feed, importer properties, etc.
  *
  */
  
$plugin = array(
  'form' => 'MODULE_MY_PLUGIN_form',
  // Optional validation callback.
  'validate' => 'MODULE_MY_PLUGIN_validate',
  'callback' => 'MODULE_MY_PLUGIN_callback',
  'name' => 'My plugin',
  'multi' => 'loop',
  'category' => 'Other',
  'filepath' => '/' . backdrop_get_path('module', 'MODULE_MY_PLUGIN') . '/plugins/myplugin.inc',
);

function MODULE_MY_PLUGIN_form($importer, $element_key, $settings) {
  $form = array();
  $form['help']['#value'] = t('My plugin can do awesome things.');
  //
  // Other formy stuff here.
  //
  return $form;
}

function MODULE_MY_PLUGIN_validate(&$settings) {
  // Validate $settings.
}

function MODULE_MY_PLUGIN_callback($feeds_parser_result, $item_key, $element_key, &$field, $settings, $feeds_source) {
  $field = crazy_modification($field);
}
?>
