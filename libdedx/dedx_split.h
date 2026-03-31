#ifndef DEDX_SPLIT_H_INCLUDED
#define DEDX_SPLIT_H_INCLUDED

/** @brief Split a fixed-width line buffer into heap-allocated tokens.
 *  @param[in]  string           Input buffer to split.
 *  @param[in]  split_character  Delimiter character.
 *  @param[out] items            Number of parsed tokens.
 *  @param[in]  str_length       Maximum number of characters to scan in @p string.
 *  @return Array of 10 heap-allocated token buffers, or NULL on allocation failure.
 *
 *  Each token buffer has room for up to 20 characters including the terminator.
 *  The returned array must be released with dedx_internal_free_split_temp().
 */
char **dedx_internal_split(char *string, char split_character, unsigned int *items, unsigned int str_length);

/** @brief Free a token array returned by dedx_internal_split().
 *  @param[in] temp   Token array to free; NULL is allowed.
 *  @param[in] count  Number of allocated token buffers in @p temp.
 */
void dedx_internal_free_split_temp(char **temp, unsigned int count);

#endif // DEDX_SPLIT_H_INCLUDED
