#!/usr/bin/env python3
"""
生成 test_expressions.cpp 的测试数据的脚本

使用方法：
见 `python3 generate_test_expressions_data.py --help` 的结果

:version: 0.1.0-2
"""

from pathlib import Path
import string
import subprocess
import helper_base


def generate_data(exec: Path, input_file: Path, ans_file: Path):
    """
    生成一次测试数据

    :param exec: 可执行文件路径
    :type exec: Path
    :param input_file: 要生成的命令集的位置
    :type input_file: Path
    :param answer_file: 要生成的答案的位置
    :type answer_file: Path
    """

    with open(input_file, "w", encoding="utf-8") as f:
        import random

        message_cnt: int = random.randint(10, 100)
        types: list[str] = ["alge", "num"]
        cmds: list[str] = [
            "new",
            "delete",
            "operator",
            "clear",
            "calculate_approximation",
            "change_to_opposite",
            "compare",
        ]
        # todo : 添加其他命令
        unary_operators: list[str] = ["front++", "front--", "back++", "back--"]
        special_operators: list[str] = ["+", "-"]
        operators: list[str] = [
            "+=",
            "-=",
            "*=",
            "/=",
            "+",
            "-",
            "*",
            "/",
            "front++",
            "front--",
            "back++",
            "back--",
        ]
        expression_cnts: dict[str, int] = {"alge": 0, "num": 0}
        variable_values: dict[str, int] = {}
        variables: list[str] = []

        for i in range(message_cnt):
            type_str: str = random.choice(types)
            cmd_str: str = random.choice(cmds + ["operator" for i in range(15)])

            if cmd_str in ["new", "delete", "clear", "change_to_opposite"]:
                if cmd_str == "delete" and expression_cnts[type_str] == 0:
                    continue
                f.write(
                    f"{type_str} {cmd_str} {random.randint(0, expression_cnts[type_str])}\n"
                )
                if cmd_str == "new":
                    expression_cnts[type_str] += 1
                elif cmd_str == "delete":
                    expression_cnts[type_str] -= 1
            elif cmd_str == "compare":
                f.write(
                    f"{type_str} {cmd_str} {random.randint(0, expression_cnts[type_str])} {random.randint(0, expression_cnts[type_str])}\n"
                )
            elif cmd_str == "operator":
                index: int = random.randint(0, expression_cnts[type_str])
                op: str = random.choice(operators)
                is_unary_operators: bool = False
                if op in special_operators:
                    is_unary_operators = bool(random.getrandbits(1))
                if op in unary_operators or is_unary_operators:
                    # 是一元操作符
                    f.write(f"{type_str} {cmd_str} {index} {op}\n")
                else:  # 是二元操作符
                    if type_str == "alge":
                        var_type: str = random.choice(
                            ["constant", "variable", "algeexpr"]
                        )
                        if var_type == "constant":
                            constant: int = random.randint(0, 100)
                            if op in ["/", "/="]:
                                if constant == 0:
                                    constant = random.randint(0, 100)
                            f.write(
                                f"{type_str} {cmd_str} {index} {op} {var_type} {constant}\n"
                            )
                        elif var_type == "variable":
                            variable: str = (
                                "".join(
                                    random.choices(
                                        string.ascii_lowercase, k=random.randint(1, 10)
                                    )
                                )
                                if len(variables) == 0 or bool(random.getrandbits(1))
                                else random.choice(variables)
                            )
                            constant: int = random.randint(0, 100)
                            if op in ["/", "/="]:
                                if constant == 0:
                                    constant = random.randint(0, 100)
                            variable_values[variable] = constant
                            variables.append(variable)
                            f.write(
                                f"{type_str} {cmd_str} {index} {op} {var_type} {variable}\n"
                            )
                        else:
                            expridx: int = random.randint(0, expression_cnts[type_str])
                            # todo : 添加除以0的错误检查
                            f.write(
                                f"{type_str} {cmd_str} {index} {op} {var_type} {expridx}\n"
                            )
                    else:
                        should_be_expr: bool = bool(random.getrandbits(1))
                        if should_be_expr:
                            expridx: int = random.randint(0, expression_cnts[type_str])
                            # todo : 添加除以0的错误检查
                            f.write(
                                f"{type_str} {cmd_str} {index} {op} numexpr {expridx}\n"
                            )
                        else:
                            constant: int = random.randint(0, 100)
                            if op in ["/", "/="]:
                                if constant == 0:
                                    constant = random.randint(0, 100)
                            f.write(f"{type_str} {cmd_str} {index} {op} {constant}\n")
            else:  # cmd_str == "calculate_approximation"
                index: int = random.randint(0, expression_cnts[type_str])
                if type_str == "alge":
                    f.write(
                        f"{type_str} {cmd_str} {index} {" ".join([f"{key} {value}" for key, value in variable_values.items()])}\n"
                    )
                else:
                    f.write(f"{type_str} {cmd_str} {index}\n")
        f.write("quit\n")
    with open(input_file, "r", encoding="utf-8") as f_in, open(
        ans_file, "w", encoding="utf-8"
    ) as f_out_err:
        subprocess.run(
            [str(exec), "--simplify-output"],
            text=True,
            stdin=f_in,
            stdout=f_out_err,
            stderr=subprocess.STDOUT,
            encoding="utf-8",
            timeout=1000,
        )


def main():
    """
    主函数
    """

    try:
        import argparse

        # 解析参数
        parser = argparse.ArgumentParser(
            description="生成 test_expressions.cpp 的测试数据的脚本"
        )
        parser.add_argument(
            "-ef",
            "--exec-file",
            type=str,
            help="需要生成数据的交互式的可执行文件路径（即 test_expressions.cpp）",
        )
        parser.add_argument(
            "-od",
            "--output-directory",
            type=str,
            help="存储生成的命令集和答案的文件夹",
        )
        parser.add_argument(
            "--times",
            type=int,
            default=1,
            help="生成命令集和答案的次数",
        )
        args = parser.parse_args()
        exec: Path = helper_base.filter_invalid_path(
            Path(args.exec_file), helper_base.PathType.FILE
        )
        output_directory: Path = helper_base.filter_invalid_path(
            Path(args.output_directory), helper_base.PathType.DIRECTORY
        )

        for i in range(1, args.times + 1):
            print(f"正在生成第{i}组数据 ...")
            generate_data(
                exec,
                output_directory / f"tests{i}.in",
                output_directory / f"tests{i}.ans",
            )
    except Exception as e:
        print(f"error: {e}")


if __name__ == "__main__":
    main()
