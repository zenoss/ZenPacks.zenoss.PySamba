#!/usr/bin/perl -w

# A perl script to connect to a VMware server and revert a VM snapshot.
# Copyright Brad Henry <brad@samba.org> 2006
# Released under the GNU GPL v2 or later.

use VMHost;

sub check_error {
my $vm = VMHost;
	my $custom_err_str = "";
	($vm, $custom_err_str) = @_;

	my ($err_code, $err_str) = $vm->error;
	if ($err_code != 0) {
		undef $vm;
		die $custom_err_str . "Returned $err_code: $err_str.\n";
	}
}

# Read in parameters from environment.
my $vm_cfg_path = $ENV{'VM_CFG_PATH'};
my $host_server_name = $ENV{'HOST_SERVER_NAME'};
my $host_server_port = $ENV{'HOST_SERVER_PORT'};
if (!defined($host_server_port)) {
	$host_server_port = 902;
}

my $host_username = $ENV{'HOST_USERNAME'};
my $host_password = $ENV{'HOST_PASSWORD'};
my $guest_admin_username = $ENV{'GUEST_ADMIN_USERNAME'};
my $guest_admin_password = $ENV{'GUEST_ADMIN_PASSWORD'};

my $vm = VMHost;

$vm->host_connect($host_server_name, $host_server_port, $host_username,
			$host_password, $vm_cfg_path, $guest_admin_username,
			$guest_admin_password);
check_error($vm, "Error in \$vm->host_connect().\n");

$vm->revert_snapshot();
check_error($vm, "Error in \$vm->revert_snapshot().\n");

undef $vm;

exit 0;
