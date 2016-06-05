/* UOL Messenger
 * Copyright (c) 2005 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 * sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 * qualquer versão posterior.
 * Este programa é distribuído na expectativa de que seja útil, porém,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 * OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 * com este programa; se não, escreva para a Free Software Foundation, Inc.,
 * no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Messenger 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */
 
#include <glib.h>
#include <debug.h>
#ifdef _WIN32
#include "win32dep.h"
#endif
#include "uimeventloop.h"

#define PURPLE_GLIB_READ_COND  (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define PURPLE_GLIB_WRITE_COND (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)

typedef struct _PurpleGlibIOClosure {
	PurpleInputFunction function;
	guint result;
	gpointer data;

} PurpleGlibIOClosure;

static void purple_glib_io_destroy(gpointer data)
{
	g_free(data);
}

static gboolean purple_glib_io_invoke(GIOChannel *source, GIOCondition condition, gpointer data)
{
	PurpleGlibIOClosure *closure = (PurpleGlibIOClosure*) data;
	PurpleInputCondition purple_cond = (PurpleInputCondition) 0;

	if (condition & PURPLE_GLIB_READ_COND)
		purple_cond = (PurpleInputCondition) (purple_cond | PURPLE_INPUT_READ);
	if (condition & PURPLE_GLIB_WRITE_COND)
		purple_cond = (PurpleInputCondition) (purple_cond | PURPLE_INPUT_WRITE);

#if 0
	purple_debug(PURPLE_DEBUG_MISC, "uim_eventloop",
			   "CLOSURE: callback for %d, fd is %d\n",
			   closure->result, g_io_channel_unix_get_fd(source));
#endif
	
#ifdef _WIN32
	if (!purple_cond)
	{
#if DEBUG
		purple_debug(PURPLE_DEBUG_MISC, "uim_eventloop",
			"CLOSURE received GIOCondition of 0x%x, which does not"
			" match 0x%x (READ) or 0x%x (WRITE)\n",
			condition, PURPLE_GTK_READ_COND, PURPLE_GTK_WRITE_COND);
#endif
		return TRUE;
	}
#endif

	closure->function(closure->data, g_io_channel_unix_get_fd(source), purple_cond);

	return TRUE;
}

guint purple_glib_input_add(gint fd, PurpleInputCondition condition, PurpleInputFunction function,
							   gpointer data)
{
	PurpleGlibIOClosure *closure = g_new0(PurpleGlibIOClosure, 1);
	GIOChannel *channel;
	GIOCondition cond = (GIOCondition) 0;

	closure->function = function;
	closure->data = data;

	if (condition & PURPLE_INPUT_READ)
		cond = (GIOCondition) (cond | PURPLE_GLIB_READ_COND);
	if (condition & PURPLE_INPUT_WRITE)
		cond = (GIOCondition) (cond | PURPLE_GLIB_WRITE_COND);

#ifdef _WIN32
	channel = wpurple_g_io_channel_win32_new_socket(fd);
#else
	channel = g_io_channel_unix_new(fd);
#endif

	closure->result = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, cond,
					      purple_glib_io_invoke, closure, purple_glib_io_destroy);

#if 0
	purple_debug(PURPLE_DEBUG_MISC, "uim_eventloop",
			   "CLOSURE: adding input watcher %d for fd %d\n",
			   closure->result, fd);
#endif

	g_io_channel_unref(channel);

	return closure->result;
}