/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 14:24:10 by agoldber          #+#    #+#             */
/*   Updated: 2025/05/15 14:15:44 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <limits.h>
# include <stdarg.h>
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 2147483647
# endif
# ifdef BUFFER_SIZE
#  if BUFFER_SIZE > MAX_INT
#   undef BUFFER_SIZE
#   define BUFFER_SIZE 3
#  endif
# endif

//BASCIC FUNCTIONS
int		ft_isalpha(int c);
int		ft_isdigit(int c);
int		ft_isalnum(int c);
int		ft_isascii(int c);
int		ft_isprint(int c);
int		ft_isspace(char *str);
int		ft_isnum(char *str);
size_t	ft_strlen(const char *s);
void	*ft_memset(void *b, int c, size_t len);
void	ft_bzero(void *s, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_memmove(void *dst, const void *src, size_t len);
size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize);
size_t	ft_strlcat(char *dst, const char *src, size_t dstsize);
int		ft_toupper(int c);
int		ft_tolower(int c);
char	*ft_strchr(const char *s, int c);
char	*ft_strrchr(const char *s, int c);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
void	*ft_memchr(const void *s, int c, size_t n);
int		ft_memcmp(const void *s1, const void *s2, size_t n);
char	*ft_strnstr(const char *haystack, const char *needle, size_t len);
int		ft_atoi(const char *str);
void	*ft_calloc(size_t count, size_t size);
char	*ft_strdup(const char *s1);
//ADVANCED FUNCTIONS
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strtrim(char const *s1, char const *set);
char	**ft_split(char const *s, char c);
char	*ft_itoa(int n);
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void	ft_striteri(char *s, void (*f)(unsigned int, char*));
void	ft_putchar_fd(char c, int fd);
void	ft_putstr_fd(char *s, int fd);
void	ft_putendl_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);
//BONUS FUNCTIONS
int		ft_atoi_base(const char *s);
//FT PRINTF
int		ft_printf(const char *format, ...);
int		ft_putchar_mod(char c);
int		print_null(void);
int		ft_putstr_mod(char *s);
int		ft_putnbr_mod(int n);
int		ft_putunbr_mod(unsigned int n);
char	*hexa_transfo(unsigned long ptr_val);
int		display_address(unsigned long ptr_val);
int		display_hexa(int n, char c);
//GNL
char	*ft_strjoin_mod(char *s1, char *s2);
int		ft_findchar(char c, char *str);
char	*get_next_line(int fd);
//ADDITIONAL FUNCTIONS
void	ft_swap(int *i, int *j);
void	free_array(char **arr);
void	ft_free(void *pointer);
#endif
