from __future__ import annotations

import json
import os
import subprocess
import sys
from os.path import abspath, expanduser
from typing import NamedTuple, Optional


class Parameter(NamedTuple):
    name: str
    possible_values: list

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return f"{self.name}: {self.possible_values}"


class Test:
    parameters: list[Parameter]
    environment_variables: list[Parameter]
    command: str
    result_extraction_properties: dict
    failed_conditions: list[str]
    csv_output: list[str]
    test_program_path: str

    _results: dict

    def __init__(
            self,
            parameters: list[Parameter],
            environment_variables: list[Parameter],
            command: str,
            result_extraction_properties: dict,
            failed_conditions: list[str],
            csv_output: list[str],
            test_program_path: str
    ):
        self.parameters = parameters
        self.environment_variables = environment_variables
        self.command = command
        self.result_extraction_properties = result_extraction_properties
        self.failed_conditions = failed_conditions
        self.csv_output = csv_output
        self.test_program_path = test_program_path
        self._results = dict()

    def __generate_parameter_table(self) -> dict:
        return {p.name: p.possible_values[0] for p in self.parameters}

    def __generate_command(self):
        output = self.command
        for name, value in self.__generate_parameter_table().items():
            output = output.replace(f"{{{name}}}", f"{value}")
        return output

    def __extract_test_results(self, output: str) -> Optional[dict]:
        for condition in self.failed_conditions:
            if condition in output:
                # A failed condition has been matched, the test is marked as failed
                return None

        lines: list[str] = output.split("\n")

        for parameter, properties in self.result_extraction_properties.items():
            substring = properties.get("substring", None)
            index = properties.get("index", None)
            if substring is None or index is None:
                print(f"Error while reading {parameter}: {properties}.")
                continue
            line = next((line for line in lines if substring in line), None)
            if line is None:
                print(f"Unable to extract result {parameter} from output.")
                continue
            words = line.split(" ")
            if len(words) <= index:
                print(f"Unable to extract index {index} of [{words}]")
                continue
            self._results[parameter] = words[index]
        return self._results

    def run(self) -> Optional[dict]:
        command = self.__generate_command()
        command_list = command.split(" ")

        # create a copy of the current system environment variables
        environment_variables = dict(os.environ)

        # populate with the environment variables for the test
        for ev in self.environment_variables:
            environment_variables[ev.name] = f"{ev.possible_values[0]}"

        # run the command with the new environment variables set
        result = subprocess.run(
            command_list,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            env=environment_variables,
            cwd=self.test_program_path
        )
        return self.__extract_test_results(result.stdout.decode('utf-8'))

    def get_csv_header(self) -> str:
        return ", ".join(self.csv_output)

    def get_csv_data(self) -> str:
        output = ""
        complete_map = self.__generate_parameter_table()
        complete_map.update({ev.name: ev.possible_values[0] for ev in self.environment_variables})
        complete_map.update(self._results)

        for output_parameter in self.csv_output:
            output += f"{complete_map.get(output_parameter, None)}, "
        return output

    def test_extract_test_results(self, output: str):
        return self.__extract_test_results(output)

    def __copy__(self):
        return Test(
            self.parameters.copy(),
            self.environment_variables.copy(),
            self.command,
            self.result_extraction_properties,
            self.failed_conditions,
            self.csv_output,
            self.test_program_path
        )

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return self.__generate_command()


def load_test_configuration(file: str) -> dict:
    with open(file) as f:
        data = json.load(f)
    return data


def build_permutations(test: Test) -> list[Test]:
    permutations = list()
    parameter_to_be_unravelled = next(
        (p for p in test.parameters if len(p.possible_values) > 1),
        None
    )
    environment_variable_to_be_unravelled = next(
        (v for v in test.environment_variables if len(v.possible_values) > 1),
        None
    )

    if environment_variable_to_be_unravelled is None and parameter_to_be_unravelled is None:
        # reached the end of the unravelling, return only the received permutation
        return [test]

    elif environment_variable_to_be_unravelled is None:
        # only parameters need to be unravelled
        for possible_value in parameter_to_be_unravelled.possible_values:
            # create a copy of the parameters we received
            test_copy = test.__copy__()

            # remove the parameter to be unravelled
            test_copy.parameters.remove(parameter_to_be_unravelled)

            # add a copy of it with only one possible value
            test_copy.parameters.append(
                Parameter(parameter_to_be_unravelled.name, [possible_value])
            )

            # append the new resulting permutations
            permutations += build_permutations(test_copy)
    else:
        # there are still environment variables that need to be unravelled
        for possible_value in environment_variable_to_be_unravelled.possible_values:
            # create a copy of the parameters we received
            test_copy = test.__copy__()

            # remove the parameter to be unravelled
            test_copy.environment_variables.remove(environment_variable_to_be_unravelled)

            # add a copy of it with only one possible value
            test_copy.environment_variables.append(
                Parameter(environment_variable_to_be_unravelled.name, [possible_value])
            )

            # append the new resulting permutations
            permutations += build_permutations(test_copy)

    # return the resulting permutations
    return permutations


def resolve_path(path: Optional[str]) -> Optional[str]:
    if path is None:
        return None

    result = path
    if "~" in path:
        result = expanduser(path)
    return abspath(result)


def usage():
    print(f"Usage: python {sys.argv[0]} <url>")


def main():
    # Read and validate the parameters
    if len(sys.argv) < 2:
        usage()
        exit(1)

    url = sys.argv[1]

    configuration = load_test_configuration("test-configuration.json")

    environment_variables = [
        Parameter(name, values)
        for name, values in configuration.get("environment_variables", dict()).items()
    ]

    parameters = [
        Parameter(name, values)
        for name, values in configuration.get("parameters", dict()).items()
    ]
    # insert url as parameter into the list
    parameters.append(Parameter("url", [url]))

    result_extraction_properties = configuration.get("results", dict())
    failed_conditions = configuration.get("failed_conditions", list())
    csv = configuration.get("csv", dict())
    original_csv_path = csv.get("path", None)
    csv_path = resolve_path(original_csv_path)
    csv_sequence = csv.get("sequence", None)

    command = configuration.get("command", None)

    original_test_program_path = configuration.get("test_program_path", None)
    test_program_path = resolve_path(original_test_program_path)

    # verifications
    if csv_path is None:
        print(f"Error: unable to resolve path {original_csv_path}. Aborting")
        exit(1)
    if test_program_path is None:
        print(f"Error: unable to resolve path {original_test_program_path}. Aborting")
        exit(2)

    test_template = Test(
        parameters,
        environment_variables,
        command,
        result_extraction_properties,
        failed_conditions,
        csv_sequence,
        test_program_path
    )
    test_suite = build_permutations(test_template)

    with open(csv_path, "w+") as f:
        # write the CSV header
        f.write(f"{test_template.get_csv_header()}\n")

        print(f"Running test suite comprised of {len(test_suite)} permutations")
        for i, test in enumerate(test_suite):
            test_results = test.run()
            if test_results is None:
                print(f"{i:03} - Failed")
            else:
                output = ""
                for name, value in test_results.items():
                    output += f"{name: <12}: {value: <10} "
                print(f"{i:03} - {output}")
            # Write CSV test result data
            f.write(f"{test.get_csv_data()}\n")

    print(f"Results have been written to {csv_path}")
    print("Done")


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()
