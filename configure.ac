dnl ---------------------------------------------------------------------
dnl Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
dnl
dnl This program is free software: you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License
dnl as published by the Free Software Foundation: either version 2
dnl of the License, or (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl ---------------------------------------------------------------------

AC_CONFIG_MACRO_DIR([m4])

AC_INIT([argon2], [0.1], [])
LT_INIT
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CC
AM_PROG_AS
AX_PTHREAD

AC_CANONICAL_HOST

AS_CASE([$host_cpu],
    dnl [i?86], [ARCH=i386],
    [x86_64], [ARCH=x86_64],
    [ARCH=generic
    AC_MSG_WARN("No code for architecture $host_cpu; using generic implementation")]
)
AC_SUBST([ARCH])

AM_CONDITIONAL([ARCH_X86_64],  [test "$ARCH" = 'x86_64'])
AM_CONDITIONAL([ARCH_GENERIC], [test "$ARCH" = 'generic'])

AC_CONFIG_FILES([
    Makefile
])

AC_OUTPUT