void free_ast_expr_tail(struct ast_expr_tail *tail)
{
  if (tail) { return; }
  free_ast_expr_tail(tail->next);
  
  if (!tail->next) { return; }
  free(tail->next);
}

void free_ast_expr(struct ast_expr *expr)
{
  free_ast_expr_tail(expr->tail);
}

void free_ast_assignment(struct ast_assignment *assign)
{
  free_ast_expr(&assign->expr);
}

void free_ast_line(struct ast_line *line)
{
  free_ast_assignment(&line->assignment);
}

void free_ast(struct ast_body *root)
{
  for (int i=0; i<root->num_lines; i++)
  {
    free_ast_line(&root->lines[i]);
  }

  free(root->lines);
}
