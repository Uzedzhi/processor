#ifndef PROC_CMDS_H
#define PROC_CMDS_H


bool do_command(proc_t *proc, bool *is_first_draw, size_t *num_of_line, size_t *draw_clock);
void print_frame(proc_t *proc, size_t width, size_t height);
void print_memory(proc_t *proc);
size_t ns_now(void);
#endif // PROC_CMDS_H