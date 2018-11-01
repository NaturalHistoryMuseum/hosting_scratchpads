<?php

/**
 * @file
 * Scratchpads drush module
 * Handles the addition of the site_data to the site context
 * The module name is the same as the feature name (scratchpads)
 */

/**
 * Implements hook_drush_init().
 */
function scratchpads_drush_init() {
  scratchpads_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function scratchpads_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Implements hook_provision_services().
 *
 * Expose the service type this extension defines to provision.
 *
 * @return array
 *   An array with the service type as key,
 *   and the default implementation the value.
 */
function scratchpads_provision_services() {
  scratchpads_provision_register_autoload();
  return array('scratchpads' => NULL);
}

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_scratchpads_post_provision_install() {
  drush_log('site_data received from the frontend (d()->site_data): ' . d()->site_data);
}

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_scratchpads_post_provision_verify() {
  drush_log('site_data received from the frontend (d()->site_data): ' . d()->site_data);
}
