#!/usr/bin/env bash

thirdparty_module_name='PNG'

upstream_git_url='git://github.com/dzenanz/libpng'
upstream_git_branch='master'

snapshot_author_name='LIBPNG Upstream'
snapshot_author_email='png-mng-implement@lists.sourceforge.net'

snapshot_redact_cmd=''
snapshot_relative_path='src/itkpng'
snapshot_paths='
  png*.c
  png*.h
  LICENSE
  scripts/pnglibconf.h.prebuilt
  arm
  intel
  mips
  powerpc
  '

source "${BASH_SOURCE%/*}/../../../Utilities/Maintenance/UpdateThirdPartyFromUpstream.sh"
update_from_upstream
