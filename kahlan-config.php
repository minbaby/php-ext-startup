<?php


/** @var \Kahlan\Cli\CommandLine $commandLine */
$commandLine = $this->commandLine();
$commandLine->option('reporter', 'default', 'verbose');
$commandLine->option('ff', 'default', 1);
isset($_SERVER['CI']) && $_SERVER['CI'] === true && $commandLine->option('clover', 'default', 'coverage.xml');
