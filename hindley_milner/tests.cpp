#include <string_view>
#include <utility>
#include <vector>

#include "expr_parser.hpp"
#include "gtest/gtest.h"
#include "type_infer.hpp"

TEST(TypeInference, AllTests) {
    const std::vector<std::pair<std::string_view, std::string_view>> test_cases = {
        {"(define f (lambda (g) (lambda (xs)"
         "(case xs (nil nil)"
         "((cons x xs1) (cons (g x) ((f g) xs1)))))))",
         "(forall (a b) ((func a b) (list a) (list b)))"},
        {"(define f (lambda (x) x))", "(forall (a) (a a))"},
        {"(define f (lambda (x) (lambda (y) x)))", "(forall (a b) (a b a))"},
        {"(define f (lambda (xs) (lambda (g)"
         "(case xs (nil nil)"
         "((cons x xs1) (cons (g x) ((f xs1) g)))))))",
         "(forall (a b) ((list a) (func a b) (list b)))"},
        {"(define f (lambda xs (case xs (nil nil))))",
         "(forall (a b) ((list a) (list b)))"},
        {"(define f (lambda g (lambda x (g x))))", "(forall (a b) ((func a b) a b))"},
        {"(define f (lambda x (lambda g (g x))))", "(forall (a b) (a (func a b) b))"},
        {"(define f (lambda g (lambda x (case x (nil (g x))))))",
         "(forall (a b) ((func (list a) b) (list a) b))"},
        {"(define f (lambda (g) (lambda (xs)"
         "(case xs (nil (g nil))"
         "((cons x xs1) (g (cons x xs1)))))))",
         "(forall (a b) ((func (list a) b) (list a) b))"},
        {"(define f (lambda g (lambda x (case x (nil nil)))))",
         "(forall (a b c) (a (list b) (list c)))"},
        {"(define f (lambda g (lambda x (lambda y (y (g x))))))",
         "(forall (a b c) ((func a b) a (func b c) c))"},
        {"(define f (lambda x (x)))", "(forall (a) (a a))"}

    };

    TypeInferSolver solver;
    SExpressionParser parser;
    for (const auto& [input, expected] : test_cases) {
        SCOPED_TRACE("Input: " + std::string(input));
        std::string actual = solver.infer_type_string(parser.parse(input));
        EXPECT_EQ(actual, expected);
    }
}
