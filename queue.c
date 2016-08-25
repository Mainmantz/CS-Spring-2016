/* queue.c
 *
 * A circular queue data structure
 *
 * Filename: queue.c
 * Description:
 * Author: Bryce Himebaugh
 * Maintainer:
 * Created: Thu Feb  7 19:49:26 2013 (-0500)
 * Version:
 * Last-Updated: 4/8/2016
 *           By: Brandon Burzon
 *     Update #: 0
 * URL:
 * Doc URL:
 * Keywords:
 * Compatibility:
 *
 */

/* Commentary:
 *
 *
 *
 */

/* Change Log:
 *
 *
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#include "queue.h"

void init_queue(queue_t *q) {
  q->head = 1;
  q->tail = 0;
  int i;
  for (i = 0; i < QUEUE_SIZE; i++)
    q->buffer[i] = 0;
}

int enqueue (queue_t *q, int data) {
  if (q->head != q->tail) {
    q->buffer[q->head] = data;
    q->head = (q->head + 1) % QUEUE_SIZE;
    return 1;
  } else {
    return 0;
  }
}

int dequeue (queue_t *q) {
  if (!queue_empty(q)) {
    int i = (q->tail + 1) % QUEUE_SIZE;
    int res = q->buffer[i];
    q->tail = i;
    return res;
  } else {
    return 0;
  }
}

int queue_empty(queue_t *q) {
  int new_tail = (q->tail + 1) % QUEUE_SIZE;
  return new_tail == q->head;
}


/* queue.c ends here */
