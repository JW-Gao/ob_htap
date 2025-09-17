/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg)
{
  std::unique_ptr<ParsedSqlNode> error_sql_node = std::make_unique<ParsedSqlNode>(SCF_ERROR);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

UnboundAggregateExpr *create_aggregate_expression(const char *aggregate_name,
                                           Expression *child,
                                           const char *sql_string,
                                           YYLTYPE *llocp)
{
  UnboundAggregateExpr *expr = new UnboundAggregateExpr(aggregate_name, child);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

UnboundFunctionExpr *create_function_expression(const char *function_name,
                                           std::vector<std::unique_ptr<Expression>> *child,
                                           const char *sql_string,
                                           YYLTYPE *llocp)
{
  UnboundFunctionExpr *expr = new UnboundFunctionExpr(function_name, child);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}


#line 135 "yacc_sql.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_BY = 4,                         /* BY  */
  YYSYMBOL_CREATE = 5,                     /* CREATE  */
  YYSYMBOL_DROP = 6,                       /* DROP  */
  YYSYMBOL_GROUP = 7,                      /* GROUP  */
  YYSYMBOL_ORDER = 8,                      /* ORDER  */
  YYSYMBOL_TABLE = 9,                      /* TABLE  */
  YYSYMBOL_VIEW = 10,                      /* VIEW  */
  YYSYMBOL_TABLES = 11,                    /* TABLES  */
  YYSYMBOL_INDEX = 12,                     /* INDEX  */
  YYSYMBOL_UNIQUE = 13,                    /* UNIQUE  */
  YYSYMBOL_CALC = 14,                      /* CALC  */
  YYSYMBOL_SELECT = 15,                    /* SELECT  */
  YYSYMBOL_DESC = 16,                      /* DESC  */
  YYSYMBOL_ASC = 17,                       /* ASC  */
  YYSYMBOL_SHOW = 18,                      /* SHOW  */
  YYSYMBOL_SYNC = 19,                      /* SYNC  */
  YYSYMBOL_INSERT = 20,                    /* INSERT  */
  YYSYMBOL_DELETE = 21,                    /* DELETE  */
  YYSYMBOL_UPDATE = 22,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 23,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 24,                    /* RBRACE  */
  YYSYMBOL_COMMA = 25,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 26,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 27,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 28,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 29,                     /* INT_T  */
  YYSYMBOL_STRING_T = 30,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 31,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 32,                    /* DATE_T  */
  YYSYMBOL_NULL_T = 33,                    /* NULL_T  */
  YYSYMBOL_TEXT_T = 34,                    /* TEXT_T  */
  YYSYMBOL_HELP = 35,                      /* HELP  */
  YYSYMBOL_EXIT = 36,                      /* EXIT  */
  YYSYMBOL_DOT = 37,                       /* DOT  */
  YYSYMBOL_INTO = 38,                      /* INTO  */
  YYSYMBOL_VALUES = 39,                    /* VALUES  */
  YYSYMBOL_FROM = 40,                      /* FROM  */
  YYSYMBOL_WHERE = 41,                     /* WHERE  */
  YYSYMBOL_AND = 42,                       /* AND  */
  YYSYMBOL_SET = 43,                       /* SET  */
  YYSYMBOL_ON = 44,                        /* ON  */
  YYSYMBOL_LOAD = 45,                      /* LOAD  */
  YYSYMBOL_VECTOR_T = 46,                  /* VECTOR_T  */
  YYSYMBOL_INFILE = 47,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 48,                   /* EXPLAIN  */
  YYSYMBOL_STORAGE = 49,                   /* STORAGE  */
  YYSYMBOL_FORMAT = 50,                    /* FORMAT  */
  YYSYMBOL_EQ = 51,                        /* EQ  */
  YYSYMBOL_LT = 52,                        /* LT  */
  YYSYMBOL_GT = 53,                        /* GT  */
  YYSYMBOL_LE = 54,                        /* LE  */
  YYSYMBOL_GE = 55,                        /* GE  */
  YYSYMBOL_NE = 56,                        /* NE  */
  YYSYMBOL_SUM = 57,                       /* SUM  */
  YYSYMBOL_AVG = 58,                       /* AVG  */
  YYSYMBOL_MAX = 59,                       /* MAX  */
  YYSYMBOL_MIN = 60,                       /* MIN  */
  YYSYMBOL_COUNT = 61,                     /* COUNT  */
  YYSYMBOL_NOT = 62,                       /* NOT  */
  YYSYMBOL_NULLABLE = 63,                  /* NULLABLE  */
  YYSYMBOL_IS = 64,                        /* IS  */
  YYSYMBOL_HAVING = 65,                    /* HAVING  */
  YYSYMBOL_IN = 66,                        /* IN  */
  YYSYMBOL_LIKE = 67,                      /* LIKE  */
  YYSYMBOL_OR = 68,                        /* OR  */
  YYSYMBOL_INNER = 69,                     /* INNER  */
  YYSYMBOL_JOIN = 70,                      /* JOIN  */
  YYSYMBOL_LENGTH = 71,                    /* LENGTH  */
  YYSYMBOL_ROUND = 72,                     /* ROUND  */
  YYSYMBOL_DATE_FORMAT = 73,               /* DATE_FORMAT  */
  YYSYMBOL_AS = 74,                        /* AS  */
  YYSYMBOL_L2_DISTANCE = 75,               /* L2_DISTANCE  */
  YYSYMBOL_COSINE_DISTANCE = 76,           /* COSINE_DISTANCE  */
  YYSYMBOL_INNER_PRODUCT = 77,             /* INNER_PRODUCT  */
  YYSYMBOL_WITH = 78,                      /* WITH  */
  YYSYMBOL_LIMIT = 79,                     /* LIMIT  */
  YYSYMBOL_NUMBER = 80,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 81,                     /* FLOAT  */
  YYSYMBOL_ID = 82,                        /* ID  */
  YYSYMBOL_SSS = 83,                       /* SSS  */
  YYSYMBOL_DATE_STR = 84,                  /* DATE_STR  */
  YYSYMBOL_VEC_STR = 85,                   /* VEC_STR  */
  YYSYMBOL_86_ = 86,                       /* '+'  */
  YYSYMBOL_87_ = 87,                       /* '-'  */
  YYSYMBOL_88_ = 88,                       /* '*'  */
  YYSYMBOL_89_ = 89,                       /* '/'  */
  YYSYMBOL_UMINUS = 90,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 91,                  /* $accept  */
  YYSYMBOL_commands = 92,                  /* commands  */
  YYSYMBOL_command_wrapper = 93,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 94,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 95,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 96,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 97,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 98,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 99,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 100,          /* drop_table_stmt  */
  YYSYMBOL_show_tables_stmt = 101,         /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 102,          /* desc_table_stmt  */
  YYSYMBOL_create_index_stmt = 103,        /* create_index_stmt  */
  YYSYMBOL_IDList = 104,                   /* IDList  */
  YYSYMBOL_drop_index_stmt = 105,          /* drop_index_stmt  */
  YYSYMBOL_create_table_stmt = 106,        /* create_table_stmt  */
  YYSYMBOL_attr_def_list = 107,            /* attr_def_list  */
  YYSYMBOL_attr_def = 108,                 /* attr_def  */
  YYSYMBOL_create_view_stmt = 109,         /* create_view_stmt  */
  YYSYMBOL_idlist = 110,                   /* idlist  */
  YYSYMBOL_number = 111,                   /* number  */
  YYSYMBOL_type = 112,                     /* type  */
  YYSYMBOL_insert_stmt = 113,              /* insert_stmt  */
  YYSYMBOL_value = 114,                    /* value  */
  YYSYMBOL_storage_format = 115,           /* storage_format  */
  YYSYMBOL_delete_stmt = 116,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 117,              /* update_stmt  */
  YYSYMBOL_update_values = 118,            /* update_values  */
  YYSYMBOL_update_value = 119,             /* update_value  */
  YYSYMBOL_select_stmt = 120,              /* select_stmt  */
  YYSYMBOL_calc_stmt = 121,                /* calc_stmt  */
  YYSYMBOL_from = 122,                     /* from  */
  YYSYMBOL_join_list = 123,                /* join_list  */
  YYSYMBOL_join_entry = 124,               /* join_entry  */
  YYSYMBOL_on = 125,                       /* on  */
  YYSYMBOL_as_alias = 126,                 /* as_alias  */
  YYSYMBOL_expression_list = 127,          /* expression_list  */
  YYSYMBOL_expression_with_alias = 128,    /* expression_with_alias  */
  YYSYMBOL_expression = 129,               /* expression  */
  YYSYMBOL_function_type = 130,            /* function_type  */
  YYSYMBOL_aggregate_type = 131,           /* aggregate_type  */
  YYSYMBOL_rel_attr = 132,                 /* rel_attr  */
  YYSYMBOL_relation = 133,                 /* relation  */
  YYSYMBOL_rel_list = 134,                 /* rel_list  */
  YYSYMBOL_where = 135,                    /* where  */
  YYSYMBOL_having_condition = 136,         /* having_condition  */
  YYSYMBOL_condition_list = 137,           /* condition_list  */
  YYSYMBOL_condition = 138,                /* condition  */
  YYSYMBOL_comp_op = 139,                  /* comp_op  */
  YYSYMBOL_group_by = 140,                 /* group_by  */
  YYSYMBOL_order_by_info = 141,            /* order_by_info  */
  YYSYMBOL_order_by_expression_list = 142, /* order_by_expression_list  */
  YYSYMBOL_order_by_type = 143,            /* order_by_type  */
  YYSYMBOL_limit_info = 144,               /* limit_info  */
  YYSYMBOL_sub_select = 145,               /* sub_select  */
  YYSYMBOL_load_data_stmt = 146,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 147,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 148,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 149             /* opt_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  87
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   326

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  91
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  166
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  298

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   341


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    88,    86,     2,    87,     2,    89,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    90
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   264,   264,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   296,   302,   307,   313,   319,   325,
     331,   338,   344,   352,   367,   384,   403,   406,   419,   429,
     450,   468,   489,   492,   505,   525,   540,   553,   563,   572,
     582,   591,   603,   616,   632,   633,   636,   639,   640,   641,
     642,   643,   644,   647,   659,   692,   696,   700,   706,   712,
     715,   733,   736,   743,   755,   778,   781,   792,   800,   844,
     853,   858,   864,   869,   881,   893,   898,   906,   909,   915,
     920,   933,   937,   942,   945,   948,   951,   954,   958,   961,
     968,   975,   981,   986,   991,   995,  1000,  1006,  1013,  1021,
    1024,  1027,  1030,  1033,  1036,  1044,  1047,  1050,  1053,  1056,
    1060,  1065,  1072,  1081,  1087,  1095,  1100,  1114,  1117,  1124,
    1128,  1135,  1138,  1143,  1148,  1153,  1160,  1173,  1174,  1175,
    1176,  1177,  1178,  1179,  1180,  1181,  1182,  1183,  1184,  1190,
    1193,  1202,  1205,  1214,  1220,  1234,  1237,  1241,  1248,  1250,
    1255,  1269,  1282,  1290,  1299,  1309,  1310
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "BY",
  "CREATE", "DROP", "GROUP", "ORDER", "TABLE", "VIEW", "TABLES", "INDEX",
  "UNIQUE", "CALC", "SELECT", "DESC", "ASC", "SHOW", "SYNC", "INSERT",
  "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T",
  "NULL_T", "TEXT_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "AND", "SET", "ON", "LOAD", "VECTOR_T", "INFILE", "EXPLAIN",
  "STORAGE", "FORMAT", "EQ", "LT", "GT", "LE", "GE", "NE", "SUM", "AVG",
  "MAX", "MIN", "COUNT", "NOT", "NULLABLE", "IS", "HAVING", "IN", "LIKE",
  "OR", "INNER", "JOIN", "LENGTH", "ROUND", "DATE_FORMAT", "AS",
  "L2_DISTANCE", "COSINE_DISTANCE", "INNER_PRODUCT", "WITH", "LIMIT",
  "NUMBER", "FLOAT", "ID", "SSS", "DATE_STR", "VEC_STR", "'+'", "'-'",
  "'*'", "'/'", "UMINUS", "$accept", "commands", "command_wrapper",
  "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt", "commit_stmt",
  "rollback_stmt", "drop_table_stmt", "show_tables_stmt",
  "desc_table_stmt", "create_index_stmt", "IDList", "drop_index_stmt",
  "create_table_stmt", "attr_def_list", "attr_def", "create_view_stmt",
  "idlist", "number", "type", "insert_stmt", "value", "storage_format",
  "delete_stmt", "update_stmt", "update_values", "update_value",
  "select_stmt", "calc_stmt", "from", "join_list", "join_entry", "on",
  "as_alias", "expression_list", "expression_with_alias", "expression",
  "function_type", "aggregate_type", "rel_attr", "relation", "rel_list",
  "where", "having_condition", "condition_list", "condition", "comp_op",
  "group_by", "order_by_info", "order_by_expression_list", "order_by_type",
  "limit_info", "sub_select", "load_data_stmt", "explain_stmt",
  "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-214)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     278,     2,   120,   194,   194,   -61,    32,  -214,    12,    28,
     -30,  -214,  -214,  -214,  -214,  -214,   -13,    -8,   278,    84,
      88,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,
    -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,
    -214,  -214,    29,    52,    59,   104,   139,    68,    70,     5,
    -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,  -214,
    -214,  -214,  -214,  -214,   116,  -214,  -214,  -214,   194,  -214,
    -214,  -214,   134,   -52,   137,   140,  -214,  -214,   124,  -214,
    -214,    90,    91,   123,   -33,   127,  -214,  -214,  -214,  -214,
     -20,   -14,   131,    94,    95,  -214,   135,   154,    15,    17,
    -214,   194,    98,  -214,   194,   194,   194,   194,  -214,   194,
     138,    99,   114,    -6,   143,   105,    34,  -214,   103,   107,
     175,   110,   175,   111,   150,   156,   119,  -214,  -214,   194,
    -214,  -214,  -214,  -214,    39,    39,  -214,  -214,   178,  -214,
      21,   -51,   179,  -214,   133,   143,   114,   110,   194,   194,
    -214,   155,   180,  -214,   169,   108,   183,   163,   191,   200,
    -214,   206,   148,   149,  -214,   208,  -214,  -214,   194,  -214,
      99,    99,   226,  -214,   210,  -214,    69,  -214,   -15,   194,
     105,   143,   228,  -214,  -214,  -214,  -214,  -214,  -214,     9,
     107,   211,   188,  -214,   110,   166,   160,   220,   221,  -214,
     223,  -214,   201,   244,   184,   217,  -214,  -214,  -214,  -214,
    -214,  -214,    77,   195,  -214,  -214,   194,   194,   194,   194,
      60,   180,  -214,   168,   181,  -214,   225,  -214,   183,    -2,
     209,  -214,   175,   234,   182,   186,  -214,   194,  -214,   194,
     194,   254,   194,  -214,  -214,  -214,    60,  -214,  -214,  -214,
    -214,  -214,  -214,   239,  -214,  -214,  -214,   163,   190,  -214,
     198,   249,   234,   261,  -214,  -214,  -214,   282,   212,  -214,
      50,  -214,  -214,   234,  -214,   263,   224,   194,   181,  -214,
    -214,   255,  -214,  -214,  -214,   266,     8,  -214,  -214,  -214,
     194,  -214,  -214,   265,   271,   194,  -214,  -214
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,    27,    28,    29,    25,    24,     0,     0,     0,     0,
     165,    23,    22,    15,    16,    17,    18,    10,    11,    12,
      13,    14,     8,     9,     5,     7,     6,     4,     3,    19,
      20,    21,     0,     0,     0,     0,     0,     0,     0,     0,
      69,   115,   118,   116,   117,   119,   109,   110,   111,   112,
     113,   114,    65,    66,   120,    67,    68,    70,     0,   102,
      99,    79,    89,    92,   108,     0,   101,   106,    80,    32,
      31,     0,     0,     0,     0,     0,   162,     1,   166,     2,
       0,     0,     0,     0,     0,    30,     0,     0,     0,     0,
      98,     0,     0,    87,     0,     0,     0,     0,    91,     0,
       0,     0,    82,     0,   127,     0,     0,   164,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   160,    97,     0,
     121,   122,    90,    88,    93,    94,    95,    96,     0,   104,
       0,   124,   125,    81,     0,   127,    82,     0,     0,   131,
      73,     0,    75,   163,     0,     0,    42,    71,    54,     0,
      52,     0,     0,     0,    38,     0,   107,   103,     0,   123,
       0,     0,   149,    83,     0,    63,     0,   128,   132,     0,
       0,   127,     0,    57,    58,    59,    60,    61,    62,    45,
       0,     0,     0,    40,     0,     0,     0,     0,     0,   100,
       0,   126,    85,     0,   129,     0,   137,   138,   139,   140,
     141,   142,     0,   143,   146,   145,     0,   131,   131,   131,
      77,    75,    74,     0,     0,    49,     0,    47,    42,    71,
       0,    55,     0,    36,     0,     0,   105,   131,    84,     0,
     131,   151,     0,   147,   148,   144,   136,   135,   133,   134,
      76,   161,    56,     0,    51,    43,    39,    71,     0,    53,
       0,     0,    36,     0,    86,   150,   130,     0,   158,    64,
      44,    41,    72,    36,    33,     0,     0,     0,     0,    78,
      48,     0,    46,    37,    34,     0,   155,   152,   159,    50,
     131,   157,   156,   153,     0,     0,    35,   154
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -214,  -214,   283,  -214,  -214,  -214,  -214,  -214,  -214,  -214,
    -214,  -214,  -214,  -203,  -214,  -214,    75,   117,  -214,  -143,
      30,  -214,  -214,   193,  -213,  -214,  -214,    89,   132,   -47,
    -214,  -214,   165,  -214,  -214,   174,    -3,  -214,   -49,  -214,
    -214,  -214,   145,   147,  -140,  -214,  -211,  -214,  -214,  -214,
    -214,    23,  -214,  -214,  -214,  -214,  -214,  -214,  -214
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,   261,    31,    32,   191,   156,    33,   159,
     253,   189,    34,    70,   193,    35,    36,   181,   152,    37,
      38,   112,   145,   146,   238,   108,    71,    72,    73,    74,
      75,    76,   142,   143,   150,   241,   177,   178,   216,   204,
     268,   287,   293,   279,    77,    39,    40,    41,    89
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      98,    78,    97,   119,   174,   172,   247,   248,   249,   121,
     217,    42,    43,     4,    44,    45,   256,   147,   116,   100,
       4,    79,   102,   102,   291,   292,   264,   218,    49,   266,
     103,   103,   224,   148,   104,   105,   106,   107,    50,   128,
     129,   222,   225,    80,   271,   167,   168,   192,    46,   117,
      81,   231,    83,   219,   120,   134,   135,   136,   137,   275,
     122,   140,    51,    52,    53,    54,    55,    50,    82,    84,
     283,   226,   227,   157,    85,   160,    56,    57,    58,   294,
      59,    60,    61,   280,    87,    62,    63,    64,    65,    66,
      67,    88,    68,    69,   104,   105,   106,   107,   132,   130,
     176,   104,   105,   106,   107,   131,   138,   104,   105,   106,
     107,    90,   281,   282,    62,    63,    93,    65,    66,    67,
     206,   207,   208,   209,   210,   211,   165,   106,   107,    47,
     220,   212,    48,   213,    91,   214,   215,   183,   184,   185,
     186,    92,   187,   243,   244,   175,   104,   105,   106,   107,
      95,    94,    96,    99,   188,   104,   105,   106,   107,   101,
     109,    49,   139,   110,   111,   200,   115,   246,   176,   176,
     176,    50,   113,   114,   118,   123,   124,   125,   127,   126,
     133,   141,   257,   144,   149,   259,   154,   151,   176,   155,
       4,   176,   158,   161,   162,    51,    52,    53,    54,    55,
     163,   164,   166,   171,   170,   180,   179,   182,   190,    56,
      57,    58,   192,    59,    60,    61,   194,    49,    62,    63,
      64,    65,    66,    67,   195,    68,    69,    50,   286,   196,
     197,   198,   199,   203,   205,   229,   265,   223,   230,   269,
     232,   176,   233,   234,   235,   237,   286,   236,   239,   240,
     251,    51,    52,    53,    54,    55,   242,   245,   254,   260,
     258,   252,   267,   270,   262,    56,    57,    58,   263,    59,
      60,    61,   272,   274,    62,    63,    64,    65,    66,    67,
     273,    68,    69,     1,     2,   276,   277,   284,   289,   290,
     295,   278,     3,     4,     5,   296,     6,     7,     8,     9,
      10,    86,   285,   255,    11,    12,    13,   228,   288,   153,
     250,   173,   221,    14,    15,   169,   202,   201,   297,     0,
       0,    16,     0,    17,     0,     0,    18
};

static const yytype_int16 yycheck[] =
{
      49,     4,    49,    23,   147,   145,   217,   218,   219,    23,
      25,     9,    10,    15,    12,    13,   229,    23,    51,    68,
      15,    82,    74,    74,    16,    17,   237,    42,    23,   240,
      82,    82,    23,    39,    86,    87,    88,    89,    33,    24,
      25,   181,    33,    11,   257,    24,    25,    49,    46,    82,
      38,   194,    82,    68,    74,   104,   105,   106,   107,   262,
      74,   110,    57,    58,    59,    60,    61,    33,    40,    82,
     273,    62,    63,   120,    82,   122,    71,    72,    73,   290,
      75,    76,    77,    33,     0,    80,    81,    82,    83,    84,
      85,     3,    87,    88,    86,    87,    88,    89,   101,    82,
     149,    86,    87,    88,    89,    88,   109,    86,    87,    88,
      89,    82,    62,    63,    80,    81,    12,    83,    84,    85,
      51,    52,    53,    54,    55,    56,   129,    88,    89,     9,
     179,    62,    12,    64,    82,    66,    67,    29,    30,    31,
      32,    82,    34,    66,    67,   148,    86,    87,    88,    89,
      82,    12,    82,    37,    46,    86,    87,    88,    89,    25,
      23,    23,    24,    23,    40,   168,    43,   216,   217,   218,
     219,    33,    82,    82,    47,    44,    82,    82,    24,    44,
      82,    82,   229,    69,    41,   232,    83,    82,   237,    82,
      15,   240,    82,    82,    44,    57,    58,    59,    60,    61,
      44,    82,    24,    70,    25,    25,    51,    38,    25,    71,
      72,    73,    49,    75,    76,    77,    25,    23,    80,    81,
      82,    83,    84,    85,    24,    87,    88,    33,   277,    23,
      82,    82,    24,     7,    24,    24,   239,     9,    50,   242,
      74,   290,    82,    23,    23,    44,   295,    24,     4,    65,
      82,    57,    58,    59,    60,    61,    39,    62,    33,    25,
      51,    80,     8,    24,    82,    71,    72,    73,    82,    75,
      76,    77,    82,    24,    80,    81,    82,    83,    84,    85,
      82,    87,    88,     5,     6,    24,     4,    24,    33,    23,
      25,    79,    14,    15,    16,    24,    18,    19,    20,    21,
      22,    18,    78,   228,    26,    27,    28,   190,   278,   116,
     221,   146,   180,    35,    36,   141,   171,   170,   295,    -1,
      -1,    43,    -1,    45,    -1,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,    14,    15,    16,    18,    19,    20,    21,
      22,    26,    27,    28,    35,    36,    43,    45,    48,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   105,   106,   109,   113,   116,   117,   120,   121,   146,
     147,   148,     9,    10,    12,    13,    46,     9,    12,    23,
      33,    57,    58,    59,    60,    61,    71,    72,    73,    75,
      76,    77,    80,    81,    82,    83,    84,    85,    87,    88,
     114,   127,   128,   129,   130,   131,   132,   145,   127,    82,
      11,    38,    40,    82,    82,    82,    93,     0,     3,   149,
      82,    82,    82,    12,    12,    82,    82,   120,   129,    37,
     129,    25,    74,    82,    86,    87,    88,    89,   126,    23,
      23,    40,   122,    82,    82,    43,    51,    82,    47,    23,
      74,    23,    74,    44,    82,    82,    44,    24,    24,    25,
      82,    88,   127,    82,   129,   129,   129,   129,   127,    24,
     129,    82,   133,   134,    69,   123,   124,    23,    39,    41,
     135,    82,   119,   114,    83,    82,   108,   120,    82,   110,
     120,    82,    44,    44,    82,   127,    24,    24,    25,   126,
      25,    70,   135,   123,   110,   127,   129,   137,   138,    51,
      25,   118,    38,    29,    30,    31,    32,    34,    46,   112,
      25,   107,    49,   115,    25,    24,    23,    82,    82,    24,
     127,   134,   133,     7,   140,    24,    51,    52,    53,    54,
      55,    56,    62,    64,    66,    67,   139,    25,    42,    68,
     129,   119,   135,     9,    23,    33,    62,    63,   108,    24,
      50,   110,    74,    82,    23,    23,    24,    44,   125,     4,
      65,   136,    39,    66,    67,    62,   129,   137,   137,   137,
     118,    82,    80,   111,    33,   107,   115,   120,    51,   120,
      25,   104,    82,    82,   137,   127,   137,     8,   141,   127,
      24,   115,    82,    82,    24,   104,    24,     4,    79,   144,
      33,    62,    63,   104,    24,    78,   129,   142,   111,    33,
      23,    16,    17,   143,   137,    25,    24,   142
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    91,    92,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   103,   103,   104,   104,   105,   106,
     106,   106,   107,   107,   108,   108,   108,   108,   108,   108,
     108,   108,   109,   109,   110,   110,   111,   112,   112,   112,
     112,   112,   112,   113,   113,   114,   114,   114,   114,   114,
     114,   115,   115,   116,   117,   118,   118,   119,   120,   121,
     122,   122,   123,   123,   124,   125,   125,   126,   126,   127,
     127,   128,   128,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   130,
     130,   130,   130,   130,   130,   131,   131,   131,   131,   131,
     132,   132,   132,   133,   133,   134,   134,   135,   135,   136,
     136,   137,   137,   137,   137,   137,   138,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   139,   140,
     140,   141,   141,   142,   142,   143,   143,   143,   144,   144,
     145,   146,   147,   148,   148,   149,   149
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     2,     9,    10,    13,     0,     3,     5,     8,
       6,     9,     0,     3,     5,     2,     6,     3,     6,     3,
       7,     4,     5,     8,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     5,     8,     1,     1,     1,     1,     1,
       1,     0,     4,     4,     6,     0,     3,     3,     9,     2,
       0,     2,     0,     2,     4,     0,     2,     1,     2,     1,
       3,     2,     1,     3,     3,     3,     3,     3,     2,     1,
       5,     1,     1,     4,     3,     6,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     1,     1,     3,     0,     2,     0,
       2,     0,     1,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     2,     2,     0,
       3,     0,     3,     2,     4,     0,     1,     1,     0,     2,
       3,     7,     2,     4,     3,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, sql_string, sql_result, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* commands: command_wrapper opt_semicolon  */
#line 265 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1903 "yacc_sql.cpp"
    break;

  case 24: /* exit_stmt: EXIT  */
#line 296 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 1912 "yacc_sql.cpp"
    break;

  case 25: /* help_stmt: HELP  */
#line 302 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 1920 "yacc_sql.cpp"
    break;

  case 26: /* sync_stmt: SYNC  */
#line 307 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 1928 "yacc_sql.cpp"
    break;

  case 27: /* begin_stmt: TRX_BEGIN  */
#line 313 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 1936 "yacc_sql.cpp"
    break;

  case 28: /* commit_stmt: TRX_COMMIT  */
#line 319 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 1944 "yacc_sql.cpp"
    break;

  case 29: /* rollback_stmt: TRX_ROLLBACK  */
#line 325 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 1952 "yacc_sql.cpp"
    break;

  case 30: /* drop_table_stmt: DROP TABLE ID  */
#line 331 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 1962 "yacc_sql.cpp"
    break;

  case 31: /* show_tables_stmt: SHOW TABLES  */
#line 338 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 1970 "yacc_sql.cpp"
    break;

  case 32: /* desc_table_stmt: DESC ID  */
#line 344 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 1980 "yacc_sql.cpp"
    break;

  case 33: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE ID IDList RBRACE  */
#line 353 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-6].string);
      create_index.relation_name = (yyvsp[-4].string);
      if ((yyvsp[-1].IDList) != nullptr) {
        (yyval.sql_node)->create_index.attribute_name.swap(*(yyvsp[-1].IDList));
      }
      create_index.attribute_name.push_back((yyvsp[-2].string));
      std::reverse((yyval.sql_node)->create_index.attribute_name.begin(), (yyval.sql_node)->create_index.attribute_name.end());
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
    }
#line 1999 "yacc_sql.cpp"
    break;

  case 34: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE ID IDList RBRACE  */
#line 368 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-6].string);
      create_index.relation_name = (yyvsp[-4].string);
      create_index.is_unique = 1;

      if ((yyvsp[-1].IDList) != nullptr) {
        (yyval.sql_node)->create_index.attribute_name.swap(*(yyvsp[-1].IDList));
      }
      create_index.attribute_name.push_back((yyvsp[-2].string));
      std::reverse((yyval.sql_node)->create_index.attribute_name.begin(), (yyval.sql_node)->create_index.attribute_name.end());
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
    }
#line 2020 "yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE VECTOR_T INDEX ID ON ID LBRACE ID RBRACE WITH LBRACE condition_list RBRACE  */
#line 385 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-9].string);
      create_index.relation_name = (yyvsp[-7].string);
      create_index.is_unique = 0;

      create_index.attribute_name.push_back((yyvsp[-5].string));
      free((yyvsp[-9].string));
      free((yyvsp[-7].string));
      free((yyvsp[-5].string));

      create_index.parameters.swap(*(yyvsp[-1].condition_list));
      delete (yyvsp[-1].condition_list);
    }
#line 2040 "yacc_sql.cpp"
    break;

  case 36: /* IDList: %empty  */
#line 403 "yacc_sql.y"
        {
          (yyval.IDList) =nullptr;
        }
#line 2048 "yacc_sql.cpp"
    break;

  case 37: /* IDList: COMMA ID IDList  */
#line 406 "yacc_sql.y"
                     {
        if((yyvsp[0].IDList)==nullptr){
          (yyval.IDList)= new std::vector<string>;
        }
        else{
          (yyval.IDList)=(yyvsp[0].IDList);
        }
        (yyval.IDList)->push_back((yyvsp[-1].string));
        free((yyvsp[-1].string));
      }
#line 2063 "yacc_sql.cpp"
    break;

  case 38: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 420 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2075 "yacc_sql.cpp"
    break;

  case 39: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE storage_format  */
#line 430 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-5].string);
      free((yyvsp[-5].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-2].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
        delete src_attrs;
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-3].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-3].attr_info);
      if ((yyvsp[0].string) != nullptr) {
        create_table.storage_format = (yyvsp[0].string);
        free((yyvsp[0].string));
      }
    }
#line 2100 "yacc_sql.cpp"
    break;

  case 40: /* create_table_stmt: CREATE TABLE ID AS select_stmt storage_format  */
#line 451 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-3].string);
      free((yyvsp[-3].string));

      SelectSqlNode *sqlnode = new SelectSqlNode;
      (*sqlnode) = std::move((yyvsp[-1].sql_node)->selection);
      delete (yyvsp[-1].sql_node);
      Expression * expr = new UnboundSubSelectExpr(sqlnode);
      create_table.sub_select=std::unique_ptr<Expression>(expr);
      
      if ((yyvsp[0].string) != nullptr) {
        create_table.storage_format = (yyvsp[0].string);
        free((yyvsp[0].string));
      }
    }
#line 2122 "yacc_sql.cpp"
    break;

  case 41: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE select_stmt storage_format  */
#line 469 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-6].string);
      free((yyvsp[-6].string));

      SelectSqlNode *sqlnode = new SelectSqlNode;
      (*sqlnode) = std::move((yyvsp[-1].sql_node)->selection);
      delete (yyvsp[-1].sql_node);
      Expression * expr = new UnboundSubSelectExpr(sqlnode);
      create_table.sub_select=std::unique_ptr<Expression>(expr);
      
      if ((yyvsp[0].string) != nullptr) {
        create_table.storage_format = (yyvsp[0].string);
        free((yyvsp[0].string));
      }
    }
#line 2144 "yacc_sql.cpp"
    break;

  case 42: /* attr_def_list: %empty  */
#line 489 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2152 "yacc_sql.cpp"
    break;

  case 43: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 493 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2166 "yacc_sql.cpp"
    break;

  case 44: /* attr_def: ID type LBRACE number RBRACE  */
#line 506 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string); 
      if((AttrType)(yyvsp[-3].number) == AttrType::VECTORS){
        if ((yyvsp[-1].number) <= 1000) {
          (yyval.attr_info)->length = (yyvsp[-1].number) * 4; 
        }
        else {
          (yyval.attr_info)->type = AttrType::HIGH_VECTORS;
          (yyval.attr_info)->length = (yyvsp[-1].number) * 4;
        }
      }else{
          (yyval.attr_info)->length = (yyvsp[-1].number);
      }
                 
      free((yyvsp[-4].string));
      (yyval.attr_info)->can_be_null = true;
    }
#line 2190 "yacc_sql.cpp"
    break;

  case 45: /* attr_def: ID type  */
#line 526 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].string);
      if((AttrType)(yyvsp[0].number) == AttrType::CHARS){
       (yyval.attr_info)->length = 1;           // 其他几个情况也应该加这个逻辑，但我懒得补全
      }else{
        (yyval.attr_info)->length = 4;                    
      }
      
      free((yyvsp[-1].string));
      (yyval.attr_info)->can_be_null = true;
    }
#line 2208 "yacc_sql.cpp"
    break;

  case 46: /* attr_def: ID type LBRACE number RBRACE NULLABLE  */
#line 541 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      if((AttrType)(yyvsp[-4].number) == AttrType::VECTORS){
          (yyval.attr_info)->length = (yyvsp[-2].number) * 4; 
      }else{
          (yyval.attr_info)->length = (yyvsp[-2].number);
      }
      free((yyvsp[-5].string));
      (yyval.attr_info)->can_be_null = true;
    }
#line 2225 "yacc_sql.cpp"
    break;

  case 47: /* attr_def: ID type NULLABLE  */
#line 554 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].string);
      (yyval.attr_info)->length = 4;
      free((yyvsp[-2].string));
      (yyval.attr_info)->can_be_null = true;
    }
#line 2238 "yacc_sql.cpp"
    break;

  case 48: /* attr_def: ID type LBRACE number RBRACE NULL_T  */
#line 564 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      free((yyvsp[-5].string));
      (yyval.attr_info)->can_be_null = true;
    }
#line 2251 "yacc_sql.cpp"
    break;

  case 49: /* attr_def: ID type NULL_T  */
#line 573 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].string);
      (yyval.attr_info)->length = 4;
      free((yyvsp[-2].string));
      (yyval.attr_info)->can_be_null = true;
    }
#line 2264 "yacc_sql.cpp"
    break;

  case 50: /* attr_def: ID type LBRACE number RBRACE NOT NULL_T  */
#line 583 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
      (yyval.attr_info)->length = (yyvsp[-3].number);
      free((yyvsp[-6].string));
      (yyval.attr_info)->can_be_null = false;
    }
#line 2277 "yacc_sql.cpp"
    break;

  case 51: /* attr_def: ID type NOT NULL_T  */
#line 592 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-2].number);
      (yyval.attr_info)->name = (yyvsp[-3].string);
      (yyval.attr_info)->length = 4;
      free((yyvsp[-3].string));
      (yyval.attr_info)->can_be_null = false;
    }
#line 2290 "yacc_sql.cpp"
    break;

  case 52: /* create_view_stmt: CREATE VIEW ID AS select_stmt  */
#line 604 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_VIEW);
      CreateViewSqlNode &create_view = (yyval.sql_node)->create_view;
      create_view.relation_name = (yyvsp[-2].string);
      free((yyvsp[-2].string));

      SelectSqlNode *sqlnode = new SelectSqlNode;
      (*sqlnode) = std::move((yyvsp[0].sql_node)->selection);
      delete (yyvsp[0].sql_node);
      Expression * expr = new UnboundSubSelectExpr(sqlnode);
      create_view.sub_select=std::unique_ptr<Expression>(expr);      
    }
#line 2307 "yacc_sql.cpp"
    break;

  case 53: /* create_view_stmt: CREATE VIEW ID LBRACE idlist RBRACE AS select_stmt  */
#line 617 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_VIEW);
      CreateViewSqlNode &create_view = (yyval.sql_node)->create_view;
      create_view.relation_name = (yyvsp[-5].string);
      free((yyvsp[-5].string));

      SelectSqlNode *sqlnode = new SelectSqlNode;
      (*sqlnode) = std::move((yyvsp[0].sql_node)->selection);
      delete (yyvsp[0].sql_node);
      Expression * expr = new UnboundSubSelectExpr(sqlnode);
      create_view.sub_select=std::unique_ptr<Expression>(expr);      
    }
#line 2324 "yacc_sql.cpp"
    break;

  case 56: /* number: NUMBER  */
#line 636 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2330 "yacc_sql.cpp"
    break;

  case 57: /* type: INT_T  */
#line 639 "yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::INTS); }
#line 2336 "yacc_sql.cpp"
    break;

  case 58: /* type: STRING_T  */
#line 640 "yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::CHARS); }
#line 2342 "yacc_sql.cpp"
    break;

  case 59: /* type: FLOAT_T  */
#line 641 "yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::FLOATS); }
#line 2348 "yacc_sql.cpp"
    break;

  case 60: /* type: DATE_T  */
#line 642 "yacc_sql.y"
             { (yyval.number) = static_cast<int>(AttrType::DATES); }
#line 2354 "yacc_sql.cpp"
    break;

  case 61: /* type: TEXT_T  */
#line 643 "yacc_sql.y"
             { (yyval.number) = static_cast<int>(AttrType::TEXTS); }
#line 2360 "yacc_sql.cpp"
    break;

  case 62: /* type: VECTOR_T  */
#line 644 "yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::VECTORS); }
#line 2366 "yacc_sql.cpp"
    break;

  case 63: /* insert_stmt: INSERT INTO ID VALUES expression_list  */
#line 648 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-2].string);
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[0].expression_list));
        delete (yyvsp[0].expression_list);
      }
      free((yyvsp[-2].string));
    }
#line 2380 "yacc_sql.cpp"
    break;

  case 64: /* insert_stmt: INSERT INTO ID LBRACE idlist RBRACE VALUES expression_list  */
#line 660 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-5].string);
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[0].expression_list));
        delete (yyvsp[0].expression_list);
      }
      free((yyvsp[-5].string));
    }
#line 2394 "yacc_sql.cpp"
    break;

  case 65: /* value: NUMBER  */
#line 692 "yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2403 "yacc_sql.cpp"
    break;

  case 66: /* value: FLOAT  */
#line 696 "yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2412 "yacc_sql.cpp"
    break;

  case 67: /* value: SSS  */
#line 700 "yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value(tmp);
      free(tmp);
      free((yyvsp[0].string));
    }
#line 2423 "yacc_sql.cpp"
    break;

  case 68: /* value: DATE_STR  */
#line 706 "yacc_sql.y"
              {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value(tmp,-1);
      free(tmp);
      free((yyvsp[0].string));
    }
#line 2434 "yacc_sql.cpp"
    break;

  case 69: /* value: NULL_T  */
#line 712 "yacc_sql.y"
           {
      (yyval.value) = new Value("null",-2);  // -2 表示null类型
    }
#line 2442 "yacc_sql.cpp"
    break;

  case 70: /* value: VEC_STR  */
#line 715 "yacc_sql.y"
            {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      char *tmp1 = nullptr;
      if(tmp[0] == '['){
        tmp1 = common::substr(tmp,1,strlen(tmp)-2);
        (yyval.value) = new Value(tmp1,-3);
        free(tmp1);
      }else{
        (yyval.value) = new Value(tmp,-3);
      }
      
      free(tmp);
      
      free((yyvsp[0].string));
    }
#line 2462 "yacc_sql.cpp"
    break;

  case 71: /* storage_format: %empty  */
#line 733 "yacc_sql.y"
    {
      (yyval.string) = nullptr;
    }
#line 2470 "yacc_sql.cpp"
    break;

  case 72: /* storage_format: STORAGE FORMAT EQ ID  */
#line 737 "yacc_sql.y"
    {
      (yyval.string) = (yyvsp[0].string);
    }
#line 2478 "yacc_sql.cpp"
    break;

  case 73: /* delete_stmt: DELETE FROM ID where  */
#line 744 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-1].string));
    }
#line 2492 "yacc_sql.cpp"
    break;

  case 74: /* update_stmt: UPDATE ID SET update_value update_values where  */
#line 756 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-4].string);
      
      if((yyvsp[-1].update_values) != nullptr){
        (yyval.sql_node)->update.update_values.swap(*(yyvsp[-1].update_values));
        delete (yyvsp[-1].update_values);
      }
      

      (yyval.sql_node)->update.update_values.push_back(std::move(*(yyvsp[-2].update_value)));


      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->update.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-4].string));
    }
#line 2516 "yacc_sql.cpp"
    break;

  case 75: /* update_values: %empty  */
#line 778 "yacc_sql.y"
        {
          (yyval.update_values) =nullptr;
        }
#line 2524 "yacc_sql.cpp"
    break;

  case 76: /* update_values: COMMA update_value update_values  */
#line 781 "yacc_sql.y"
                                      {
        if((yyvsp[0].update_values)==nullptr){
          (yyval.update_values)= new std::vector<UpdateValueNode>;
        }
        else{
          (yyval.update_values)=(yyvsp[0].update_values);
        }
        (yyval.update_values)->push_back(std::move(*(yyvsp[-1].update_value)));
      }
#line 2538 "yacc_sql.cpp"
    break;

  case 77: /* update_value: ID EQ expression  */
#line 792 "yacc_sql.y"
                    {
      UpdateValueNode *updatevalue =new UpdateValueNode();
      updatevalue->attribute_name = (yyvsp[-2].string);
      updatevalue->expr = std::unique_ptr<Expression>((yyvsp[0].expression));
      (yyval.update_value) = updatevalue;
    }
#line 2549 "yacc_sql.cpp"
    break;

  case 78: /* select_stmt: SELECT expression_list from join_list where group_by having_condition order_by_info limit_info  */
#line 801 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-7].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[-7].expression_list));
        delete (yyvsp[-7].expression_list);
      }

      if ((yyvsp[-6].relation_list) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-6].relation_list));
        delete (yyvsp[-6].relation_list);
      }

      if ((yyvsp[-5].join_list) != nullptr) {
        (yyval.sql_node)->selection.join_info.swap(*(yyvsp[-5].join_list));
        delete (yyvsp[-5].join_list);
      }

      if ((yyvsp[-4].condition_list) != nullptr) {
        (yyval.sql_node)->selection.conditions.swap(*(yyvsp[-4].condition_list));
        delete (yyvsp[-4].condition_list);
      }

      if ((yyvsp[-3].expression_list) != nullptr) {
        (yyval.sql_node)->selection.group_by.swap(*(yyvsp[-3].expression_list));
        delete (yyvsp[-3].expression_list);
      }
      
      if ((yyvsp[-2].condition_list) != nullptr) {
        (yyval.sql_node)->selection.group_by_conditions.swap(*(yyvsp[-2].condition_list));
        delete (yyvsp[-2].condition_list);
      }

      if ((yyvsp[-1].order_by_info) != nullptr) {
        (yyval.sql_node)->selection.order_by.order_by_attrs.swap((yyvsp[-1].order_by_info)->order_by_attrs);
        (yyval.sql_node)->selection.order_by.order_by_types.swap((yyvsp[-1].order_by_info)->order_by_types);
        delete (yyvsp[-1].order_by_info);
      }

      (yyval.sql_node)->selection.limit=(yyvsp[0].number);

    }
#line 2595 "yacc_sql.cpp"
    break;

  case 79: /* calc_stmt: CALC expression_list  */
#line 845 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2605 "yacc_sql.cpp"
    break;

  case 80: /* from: %empty  */
#line 853 "yacc_sql.y"
  {
    // empty
    (yyval.relation_list) = nullptr;
  }
#line 2614 "yacc_sql.cpp"
    break;

  case 81: /* from: FROM rel_list  */
#line 858 "yacc_sql.y"
               {
    (yyval.relation_list) = (yyvsp[0].relation_list);
  }
#line 2622 "yacc_sql.cpp"
    break;

  case 82: /* join_list: %empty  */
#line 864 "yacc_sql.y"
  {
    /*empty*/
    (yyval.join_list) = nullptr;
  }
#line 2631 "yacc_sql.cpp"
    break;

  case 83: /* join_list: join_entry join_list  */
#line 869 "yacc_sql.y"
                      {
    if ((yyvsp[0].join_list) != nullptr) {
      (yyval.join_list) = (yyvsp[0].join_list);
    }
    else {
      (yyval.join_list) = new std::vector<JoinEntry>;
    }
    (yyval.join_list)->push_back(std::move(*(yyvsp[-1].join_entry)));
  }
#line 2645 "yacc_sql.cpp"
    break;

  case 84: /* join_entry: INNER JOIN relation on  */
#line 881 "yacc_sql.y"
                         {
    (yyval.join_entry) = new JoinEntry;
    (yyval.join_entry)->join_table = *(yyvsp[-1].relation);
    delete((yyvsp[-1].relation));
    if ((yyvsp[0].condition_list) != nullptr) {
      (yyval.join_entry)->join_conditions.swap(*(yyvsp[0].condition_list));
      delete (yyvsp[0].condition_list);
    }
  }
#line 2659 "yacc_sql.cpp"
    break;

  case 85: /* on: %empty  */
#line 893 "yacc_sql.y"
  {
    /*empty*/
    (yyval.condition_list) = nullptr;
  }
#line 2668 "yacc_sql.cpp"
    break;

  case 86: /* on: ON condition_list  */
#line 899 "yacc_sql.y"
  {
    (yyval.condition_list) = (yyvsp[0].condition_list);
  }
#line 2676 "yacc_sql.cpp"
    break;

  case 87: /* as_alias: ID  */
#line 906 "yacc_sql.y"
      {
    (yyval.string) = (yyvsp[0].string);
  }
#line 2684 "yacc_sql.cpp"
    break;

  case 88: /* as_alias: AS ID  */
#line 909 "yacc_sql.y"
          {
    (yyval.string) = (yyvsp[0].string);
  }
#line 2692 "yacc_sql.cpp"
    break;

  case 89: /* expression_list: expression_with_alias  */
#line 916 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<std::unique_ptr<Expression>>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 2701 "yacc_sql.cpp"
    break;

  case 90: /* expression_list: expression_with_alias COMMA expression_list  */
#line 921 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<std::unique_ptr<Expression>>;
      }
      (yyval.expression_list)->emplace((yyval.expression_list)->begin(), (yyvsp[-2].expression));
    }
#line 2714 "yacc_sql.cpp"
    break;

  case 91: /* expression_with_alias: expression as_alias  */
#line 933 "yacc_sql.y"
                       {
      (yyvsp[-1].expression)->set_name((yyvsp[0].string));
      (yyval.expression) = (yyvsp[-1].expression);
    }
#line 2723 "yacc_sql.cpp"
    break;

  case 92: /* expression_with_alias: expression  */
#line 937 "yacc_sql.y"
                 {
      (yyval.expression) = (yyvsp[0].expression);
    }
#line 2731 "yacc_sql.cpp"
    break;

  case 93: /* expression: expression '+' expression  */
#line 942 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2739 "yacc_sql.cpp"
    break;

  case 94: /* expression: expression '-' expression  */
#line 945 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2747 "yacc_sql.cpp"
    break;

  case 95: /* expression: expression '*' expression  */
#line 948 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2755 "yacc_sql.cpp"
    break;

  case 96: /* expression: expression '/' expression  */
#line 951 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2763 "yacc_sql.cpp"
    break;

  case 97: /* expression: LBRACE expression RBRACE  */
#line 954 "yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2772 "yacc_sql.cpp"
    break;

  case 98: /* expression: '-' expression  */
#line 958 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2780 "yacc_sql.cpp"
    break;

  case 99: /* expression: value  */
#line 961 "yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2790 "yacc_sql.cpp"
    break;

  case 100: /* expression: LBRACE expression COMMA expression_list RBRACE  */
#line 968 "yacc_sql.y"
                                                     {
      (yyvsp[-1].expression_list)->emplace((yyvsp[-1].expression_list)->begin(),(yyvsp[-3].expression));
      Value values(*(yyvsp[-1].expression_list));
      (yyval.expression) = new ValueExpr(values);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[-1].expression_list); // 不用delete $2 因为 $2 包含在$4里
    }
#line 2802 "yacc_sql.cpp"
    break;

  case 101: /* expression: rel_attr  */
#line 975 "yacc_sql.y"
              {
      RelAttrSqlNode *node = (yyvsp[0].rel_attr);
      (yyval.expression) = new UnboundFieldExpr(node->relation_name, node->attribute_name);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].rel_attr);
    }
#line 2813 "yacc_sql.cpp"
    break;

  case 102: /* expression: '*'  */
#line 981 "yacc_sql.y"
          {
      (yyval.expression) = new StarExpr();
    }
#line 2821 "yacc_sql.cpp"
    break;

  case 103: /* expression: aggregate_type LBRACE expression RBRACE  */
#line 986 "yacc_sql.y"
                                             {
       (yyval.expression) = new UnboundAggregateExpr((yyvsp[-3].const_string), (yyvsp[-1].expression));
       (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2830 "yacc_sql.cpp"
    break;

  case 104: /* expression: aggregate_type LBRACE RBRACE  */
#line 991 "yacc_sql.y"
                                   {
       (yyval.expression) = new UnboundAggregateExpr("Failure", nullptr);
       (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2839 "yacc_sql.cpp"
    break;

  case 105: /* expression: aggregate_type LBRACE expression COMMA expression_list RBRACE  */
#line 995 "yacc_sql.y"
                                                                   {
       (yyval.expression) = new UnboundAggregateExpr("Failure", nullptr);
       (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2848 "yacc_sql.cpp"
    break;

  case 106: /* expression: sub_select  */
#line 1000 "yacc_sql.y"
                {
      (yyval.expression) = new UnboundSubSelectExpr((yyvsp[0].sub_select));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));

    }
#line 2858 "yacc_sql.cpp"
    break;

  case 107: /* expression: function_type LBRACE expression_list RBRACE  */
#line 1006 "yacc_sql.y"
                                                {
      (yyval.expression) = create_function_expression((yyvsp[-3].const_string), (yyvsp[-1].expression_list), sql_string, &(yyloc));
      //if ($5 != nullptr) {
      //  $$->set_name($5);
        //delete $5;
      //}
    }
#line 2870 "yacc_sql.cpp"
    break;

  case 108: /* expression: function_type  */
#line 1013 "yacc_sql.y"
                  {
      (yyval.expression) = new ValueExpr(Value((yyvsp[0].const_string)));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      //delete $1;
    }
#line 2880 "yacc_sql.cpp"
    break;

  case 109: /* function_type: LENGTH  */
#line 1021 "yacc_sql.y"
           {
      (yyval.const_string) = "length";
    }
#line 2888 "yacc_sql.cpp"
    break;

  case 110: /* function_type: ROUND  */
#line 1024 "yacc_sql.y"
            {
      (yyval.const_string) = "round";
    }
#line 2896 "yacc_sql.cpp"
    break;

  case 111: /* function_type: DATE_FORMAT  */
#line 1027 "yacc_sql.y"
                  {
      (yyval.const_string) = "date_format";
    }
#line 2904 "yacc_sql.cpp"
    break;

  case 112: /* function_type: L2_DISTANCE  */
#line 1030 "yacc_sql.y"
                  {
      (yyval.const_string) = "l2_distance";
    }
#line 2912 "yacc_sql.cpp"
    break;

  case 113: /* function_type: COSINE_DISTANCE  */
#line 1033 "yacc_sql.y"
                      {
      (yyval.const_string) = "cosine_distance";
    }
#line 2920 "yacc_sql.cpp"
    break;

  case 114: /* function_type: INNER_PRODUCT  */
#line 1036 "yacc_sql.y"
                    {
      (yyval.const_string) = "inner_product";
      }
#line 2928 "yacc_sql.cpp"
    break;

  case 115: /* aggregate_type: SUM  */
#line 1044 "yacc_sql.y"
       {
      (yyval.const_string) = "sum";
    }
#line 2936 "yacc_sql.cpp"
    break;

  case 116: /* aggregate_type: MAX  */
#line 1047 "yacc_sql.y"
       {
      (yyval.const_string) = "max";
    }
#line 2944 "yacc_sql.cpp"
    break;

  case 117: /* aggregate_type: MIN  */
#line 1050 "yacc_sql.y"
       {
      (yyval.const_string) = "min";
    }
#line 2952 "yacc_sql.cpp"
    break;

  case 118: /* aggregate_type: AVG  */
#line 1053 "yacc_sql.y"
       {
      (yyval.const_string) = "avg";
    }
#line 2960 "yacc_sql.cpp"
    break;

  case 119: /* aggregate_type: COUNT  */
#line 1056 "yacc_sql.y"
         {
      (yyval.const_string) = "count";
    }
#line 2968 "yacc_sql.cpp"
    break;

  case 120: /* rel_attr: ID  */
#line 1060 "yacc_sql.y"
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2978 "yacc_sql.cpp"
    break;

  case 121: /* rel_attr: ID DOT ID  */
#line 1065 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2990 "yacc_sql.cpp"
    break;

  case 122: /* rel_attr: ID DOT '*'  */
#line 1072 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = '*';
      free((yyvsp[-2].string));
    }
#line 3001 "yacc_sql.cpp"
    break;

  case 123: /* relation: ID as_alias  */
#line 1081 "yacc_sql.y"
                {
      (yyval.relation) = new std::pair<std::string,std::string>();
      (*(yyval.relation)) = std::make_pair(std::string((yyvsp[0].string)),std::string((yyvsp[-1].string)));
      free((yyvsp[-1].string));
      free((yyvsp[0].string));
    }
#line 3012 "yacc_sql.cpp"
    break;

  case 124: /* relation: ID  */
#line 1087 "yacc_sql.y"
      {
      (yyval.relation) = new std::pair<std::string,std::string>();
      (*(yyval.relation)) = std::make_pair(std::string((yyvsp[0].string)),std::string((yyvsp[0].string)));
      free((yyvsp[0].string));
      
    }
#line 3023 "yacc_sql.cpp"
    break;

  case 125: /* rel_list: relation  */
#line 1095 "yacc_sql.y"
             {
      (yyval.relation_list) = new std::vector<std::pair<std::string,std::string>>();
      (yyval.relation_list)->push_back(*(yyvsp[0].relation));
      delete((yyvsp[0].relation));
    }
#line 3033 "yacc_sql.cpp"
    break;

  case 126: /* rel_list: relation COMMA rel_list  */
#line 1100 "yacc_sql.y"
                              {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::pair<std::string,std::string>>();
      }

      (yyval.relation_list)->insert((yyval.relation_list)->begin(), *(yyvsp[-2].relation));
      delete((yyvsp[-2].relation));
    }
#line 3048 "yacc_sql.cpp"
    break;

  case 127: /* where: %empty  */
#line 1114 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3056 "yacc_sql.cpp"
    break;

  case 128: /* where: WHERE condition_list  */
#line 1117 "yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 3064 "yacc_sql.cpp"
    break;

  case 129: /* having_condition: %empty  */
#line 1124 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3072 "yacc_sql.cpp"
    break;

  case 130: /* having_condition: HAVING condition_list  */
#line 1128 "yacc_sql.y"
                          {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 3080 "yacc_sql.cpp"
    break;

  case 131: /* condition_list: %empty  */
#line 1135 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3088 "yacc_sql.cpp"
    break;

  case 132: /* condition_list: condition  */
#line 1138 "yacc_sql.y"
                {
      (yyvsp[0].condition)->conjunction_type = 0;
      (yyval.condition_list) = new std::vector<ConditionSqlNode>;
      (yyval.condition_list)->push_back(std::move(*(yyvsp[0].condition)));
    }
#line 3098 "yacc_sql.cpp"
    break;

  case 133: /* condition_list: condition AND condition_list  */
#line 1143 "yacc_sql.y"
                                   {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      (yyvsp[-2].condition)->conjunction_type = 0;
      (yyval.condition_list)->push_back(std::move(*(yyvsp[-2].condition)));
    }
#line 3108 "yacc_sql.cpp"
    break;

  case 134: /* condition_list: condition OR condition_list  */
#line 1148 "yacc_sql.y"
                                  {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      (yyvsp[-2].condition)->conjunction_type = 1;
      (yyval.condition_list)->push_back(std::move(*(yyvsp[-2].condition)));
    }
#line 3118 "yacc_sql.cpp"
    break;

  case 135: /* condition_list: condition COMMA condition_list  */
#line 1153 "yacc_sql.y"
                                     {       // 用来代替 vector 索引的条件
      (yyval.condition_list) = (yyvsp[0].condition_list);
      (yyvsp[-2].condition)->conjunction_type = 0;
      (yyval.condition_list)->push_back(std::move(*(yyvsp[-2].condition)));
    }
#line 3128 "yacc_sql.cpp"
    break;

  case 136: /* condition: expression comp_op expression  */
#line 1161 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_expression.emplace_back((yyvsp[-2].expression));
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->right_expression.emplace_back((yyvsp[0].expression));
      (yyval.condition)->comp = (yyvsp[-1].comp);

    }
#line 3142 "yacc_sql.cpp"
    break;

  case 137: /* comp_op: EQ  */
#line 1173 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3148 "yacc_sql.cpp"
    break;

  case 138: /* comp_op: LT  */
#line 1174 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3154 "yacc_sql.cpp"
    break;

  case 139: /* comp_op: GT  */
#line 1175 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3160 "yacc_sql.cpp"
    break;

  case 140: /* comp_op: LE  */
#line 1176 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3166 "yacc_sql.cpp"
    break;

  case 141: /* comp_op: GE  */
#line 1177 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3172 "yacc_sql.cpp"
    break;

  case 142: /* comp_op: NE  */
#line 1178 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3178 "yacc_sql.cpp"
    break;

  case 143: /* comp_op: IS  */
#line 1179 "yacc_sql.y"
         { (yyval.comp) = IS_NULL;}
#line 3184 "yacc_sql.cpp"
    break;

  case 144: /* comp_op: IS NOT  */
#line 1180 "yacc_sql.y"
             { (yyval.comp) = IS_NOT;}
#line 3190 "yacc_sql.cpp"
    break;

  case 145: /* comp_op: LIKE  */
#line 1181 "yacc_sql.y"
           { (yyval.comp) = LIKE_OP;}
#line 3196 "yacc_sql.cpp"
    break;

  case 146: /* comp_op: IN  */
#line 1182 "yacc_sql.y"
         { (yyval.comp) = IN_VALUELIST;}
#line 3202 "yacc_sql.cpp"
    break;

  case 147: /* comp_op: NOT IN  */
#line 1183 "yacc_sql.y"
             { (yyval.comp) = NOT_IN_VALUELIST;}
#line 3208 "yacc_sql.cpp"
    break;

  case 148: /* comp_op: NOT LIKE  */
#line 1184 "yacc_sql.y"
               {(yyval.comp) = NOT_LIKE_OP;}
#line 3214 "yacc_sql.cpp"
    break;

  case 149: /* group_by: %empty  */
#line 1190 "yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 3222 "yacc_sql.cpp"
    break;

  case 150: /* group_by: GROUP BY expression_list  */
#line 1194 "yacc_sql.y"
    {
      (yyval.expression_list) = (yyvsp[0].expression_list);
    }
#line 3230 "yacc_sql.cpp"
    break;

  case 151: /* order_by_info: %empty  */
#line 1202 "yacc_sql.y"
      {
        (yyval.order_by_info) = nullptr;
      }
#line 3238 "yacc_sql.cpp"
    break;

  case 152: /* order_by_info: ORDER BY order_by_expression_list  */
#line 1206 "yacc_sql.y"
      { 
        (yyval.order_by_info) = (yyvsp[0].order_by_info);
        std::reverse((yyval.order_by_info)->order_by_attrs.begin(),(yyval.order_by_info)->order_by_attrs.end());
        std::reverse((yyval.order_by_info)->order_by_types.begin(),(yyval.order_by_info)->order_by_types.end());
      }
#line 3248 "yacc_sql.cpp"
    break;

  case 153: /* order_by_expression_list: expression order_by_type  */
#line 1215 "yacc_sql.y"
    {
      (yyval.order_by_info) = new OrderByInfo();
      (yyval.order_by_info)->order_by_attrs.emplace_back((yyvsp[-1].expression));
      (yyval.order_by_info)->order_by_types.push_back((yyvsp[0].order_by_type));
    }
#line 3258 "yacc_sql.cpp"
    break;

  case 154: /* order_by_expression_list: expression order_by_type COMMA order_by_expression_list  */
#line 1221 "yacc_sql.y"
    {
      if ((yyvsp[0].order_by_info) != nullptr) {
        (yyval.order_by_info) = (yyvsp[0].order_by_info);
      } else {
        (yyval.order_by_info) = new OrderByInfo();
      }
      (yyval.order_by_info)->order_by_attrs.emplace_back((yyvsp[-3].expression));
      (yyval.order_by_info)->order_by_types.push_back((yyvsp[-2].order_by_type));
    }
#line 3272 "yacc_sql.cpp"
    break;

  case 155: /* order_by_type: %empty  */
#line 1234 "yacc_sql.y"
      {
        (yyval.order_by_type) = ASC_SORT;
      }
#line 3280 "yacc_sql.cpp"
    break;

  case 156: /* order_by_type: ASC  */
#line 1238 "yacc_sql.y"
      { 
        (yyval.order_by_type) = ASC_SORT;
      }
#line 3288 "yacc_sql.cpp"
    break;

  case 157: /* order_by_type: DESC  */
#line 1242 "yacc_sql.y"
      { 
        (yyval.order_by_type) = DESC_SORT;
      }
#line 3296 "yacc_sql.cpp"
    break;

  case 158: /* limit_info: %empty  */
#line 1248 "yacc_sql.y"
   { (yyval.number) =-1;
   }
#line 3303 "yacc_sql.cpp"
    break;

  case 159: /* limit_info: LIMIT number  */
#line 1250 "yacc_sql.y"
               {
    (yyval.number) = (yyvsp[0].number);
   }
#line 3311 "yacc_sql.cpp"
    break;

  case 160: /* sub_select: LBRACE select_stmt RBRACE  */
#line 1256 "yacc_sql.y"
  {
    (yyval.sub_select) = new SelectSqlNode;
    (*(yyval.sub_select)) = std::move((yyvsp[-1].sql_node)->selection);
    delete (yyvsp[-1].sql_node);

  }
#line 3322 "yacc_sql.cpp"
    break;

  case 161: /* load_data_stmt: LOAD ID INFILE SSS INTO TABLE ID  */
#line 1270 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 3336 "yacc_sql.cpp"
    break;

  case 162: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1283 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3345 "yacc_sql.cpp"
    break;

  case 163: /* set_variable_stmt: SET ID EQ value  */
#line 1291 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 3357 "yacc_sql.cpp"
    break;

  case 164: /* set_variable_stmt: SET ID ID  */
#line 1300 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-1].string);
      (yyval.sql_node)->set_variable.value = Value((yyvsp[0].string));
      free((yyvsp[-1].string));
      free((yyvsp[0].string));
    }
#line 3369 "yacc_sql.cpp"
    break;


#line 3373 "yacc_sql.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1312 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);
  yylex_destroy(scanner);
  return result;
}
