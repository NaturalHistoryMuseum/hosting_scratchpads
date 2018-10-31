<?php
/**
 * Implements hook_hosting_site_context_options().
 */
function hosting_scratchpads_hosting_site_context_options(&$task){
  if(module_exists('hosting_scratchpads')){
    $task = &drush_get_context('HOSTING_TASK');
    if($task->ref->type == 'site'){
      $task->options['client_name'] = $task->context_options['client_name'] = $task->ref->client_name;
      $task->options['client_email'] = $task->context_options['client_email'] = $task->ref->client_email;
      $task->context_options['site_data'] = array(
        'client_title' => $task->ref->client_title,
        'client_name' => $task->ref->client_name,
        'client_institution' => $task->ref->client_institution,
        'client_email' => $task->ref->client_email,
        'client_country' => $task->ref->client_country,
        'site_title' => $task->ref->site_title,
        'site_taxonomic_scope' => $task->ref->site_taxonomic_scope
      );

      // FIXME: The code above is supposed to add these variables to the alias config file
      // however for some reason this no longer works and the alias file is created without site_data.
      // We can't even create it manually because it gets overwritten with the version missing site_data.
      // Instead we create a separate config file with ".site_data" in its name so that we have access to
      // this information later - it's used in the provison_welcome_mail.inc (scratchpads2 repo)
      // file and I'm not sure where else, if anywhere.
      $config = new Provision_Config_Drushrc_Alias($task->ref->hosting_name.".site_data", array('site_data' => $task->context_options['site_data']));
      $config->write();
    }
  }
}


/**
 * Implements hook_drush_command().
 */
function hosting_scratchpads_drush_command() {
  $commands = array();

  $commands['remote-import-scratchpad'] = array(
    'description' => 'Import a scratchpad from a remote host.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
    'aliases' => array('remote-import-scratchpads'),
    'arguments' => array(
       'site' => 'The site(s) to import',
     ),
     'options' => array(
       'new_platform' => 'The platform to import into. Defaults to scratchpads master.',
       'remote_host' => 'The remote host to pull from, if there\'s more than one available.',
     ),
     'examples' => array(
      'drush @hm remote-import-scratchpad example.myspecies.info' => 'Import example scratchpad into master platform.',
      'drush @hm remote-import-scratchpad a.myspecies.info b.myspecies.info' => 'Import both a and b with default settings',
      'drush @hm remote-import-scratchpad example.myspecies.info --new_platform=scratchpads-2.9.3' => 'Import into 2.9.3 platform',
    )
  );

  $commands['remote-import-list-scratchpads'] = array(
    'description' => 'Print a list of sites on the remote host that are not present on this host, and their platform.',
    'aliases' => array('remote-scratchpads'),
    'examples' => array(
      'drush @hm remote-import-list-scratchpads | less' => 'Get list and pipe to less',
    ),
    'options' => array(
      'remote_host' => 'The remote host to query, if there\'s more than one available.',
    )
  );

  return $commands;
}

/**
 * Select the remote host to use, or error
 */
function hosting_scratchpads_drush_get_remote_host() {
  // If the user supplied an option, just use that
  $remote_host = drush_get_option('remote_host');

  // If no host was given, we try and pick it. If it's ambiguous
  // or there's no hosts, throw an error
  if(!$remote_host) {
    $remotes = hosting_scratchpads_get_remote_import_hosts();

    if(count($remotes) < 1) {
      echo "No remote hostmaster installed\n";
      die;
    }

    if(count($remotes) > 1) {
      echo "Too many hostmasters, please select one using --remote_host option:\n";
      print_r($remotes);
      die;
    }

    $remote_host = $remotes[0];
  }

  return $remote_host;
}

/**
 * Drush remote-import-list-scratchpads command
 */
function drush_hosting_scratchpads_remote_import_list_scratchpads() {
  $remote_host = hosting_scratchpads_drush_get_remote_host();
  $existing_sites = hosting_scratchpads_get_site_names();

  // Invoke `drush @remote_host provision-remote_import_list_sites`
  $command = provision_backend_invoke($remote_host, 'provision-remote_import_list_sites', array(), array());

  // Send each site to stdout
  foreach ($command['context']['remote_sites_list'] as $site => $platform) {
    // Ignore it if it's already on this host
    if(!in_array($site, $existing_sites)) {
      echo "$site ($platform)\n";
    }
  }
}

/**
 * Drush remote-import-scratchpad command
 * Takes a list of sites to import
 */
function drush_hosting_scratchpads_remote_import_scratchpad(...$args) {
  $remote_host = hosting_scratchpads_drush_get_remote_host();
  $platform = drush_get_option('new_platform');
  $databases = hosting_scratchpads_database_picker();
  $existing_sites = hosting_scratchpads_get_site_names();

  // Sensible default
  if(!$platform) {
    echo "No platform given, assuming scratchpasmaster. Use the --new_platform option to specify another.\n";
    $platform = 'platform_scratchpadsmaster';
  }

  // Add @platform_ to the start if it's ambiguous
  $platform = preg_replace("/^(?!@?platform_)/", "@platform_", $platform);

  foreach($args as $site) {
    // Ignore site if it's already on this host
    if(in_array($site, $existing_sites)) {
      echo "$site already exists, skipping...\n";
      continue;
    }

    // Invoke `drush @remote_host provision-remote_import --old_url=site --new_url=site --platform=@platform_whatever --db_server=@next_db_server`
    provision_backend_invoke(
      $remote_host,
      'provision-remote_import',
      array(),
      array(
        'old_url' => $site,
        'new_url' => $site,
        'platform' => $platform,
        'db_server' => $databases->current() // Get the current iterator value
      )
    );
    // Advance the iterator ready for the next loop
    $databases->next();
  }
}
