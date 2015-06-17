<?php

/**
 * Implementation of hook_pre_provision_install
 */
function drush_hostmaster_scratchpad_pre_provision_install($url = NULL){
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
function drush_hostmaster_scratchpad_pre_hosting_task(){
  $task = & drush_get_context('HOSTING_TASK');
  if($task->ref->type == 'site' && ($task->task_type == 'install' || $task->task_type == 'verify')){
    $task->options['client_title'] = $task->ref->client_title;
    $task->options['fullname'] = $task->ref->client_name;
    $task->options['institution'] = $task->ref->client_institution;
    $task->options['site_title'] = $task->ref->site_title;
    $task->options['client_email'] = $task->ref->client_title;
    $task->options['country'] = $task->ref->client_country;
    $task->options['taxonomic_scope'] = $task->ref->site_taxonomic_scope;
  }
}