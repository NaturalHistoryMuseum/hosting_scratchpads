<?php

/**
 * Implementation of hook_pre_provision_install
 */
function drush_hosting_scratchpads_pre_provision_install($url = NULL){
  drush_set_option('site_title', drush_get_option('site_title', FALSE), 'site');
  drush_set_option('client_title', drush_get_option('client_title', FALSE), 'site');
  drush_set_option('country', drush_get_option('country', FALSE), 'site');
  drush_set_option('client_email', drush_get_option('client_email', FALSE), 'site');
  drush_set_option('fullname', drush_get_option('fullname', FALSE), 'site');
  drush_set_option('institution', drush_get_option('institution', FALSE), 'site');
  drush_set_option('taxonomic_scope', drush_get_option('taxonomic_scope', FALSE), 'site');
}

/**
 * Implementation of drush_hook_pre_hosting_task()
 */
function drush_hosting_scratchpads_pre_hosting_task(){
  if(module_exists('hosting_scratchpads')){
    $task = &drush_get_context('HOSTING_TASK');
    if($task->ref->type == 'site'){
      $task->context_options['client_title'] = $task->ref->client_title;
      $task->context_options['fullname'] = $task->ref->client_name;
      $task->context_options['institution'] = $task->ref->client_institution;
      $task->context_options['site_title'] = $task->ref->site_title;
      $task->context_options['client_email'] = $task->ref->client_title;
      $task->context_options['country'] = $task->ref->client_country;
      $task->context_options['taxonomic_scope'] = $task->ref->site_taxonomic_scope;
    }
  }
}