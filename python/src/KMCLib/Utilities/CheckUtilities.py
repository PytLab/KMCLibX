""" Module for holding common checking utility functions. """


# Copyright (c)  2012-2013  Mikael Leetmaa
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#


import numpy

from KMCLib.Exceptions.Error import Error


def checkIndexWithinBounds(index, list, msg=None):
    """
    Check that the given index is within the bounds of the list.

    :param index: The index to check.
    :type index:  int

    :param list:  The list to check against.

    :param msg: The error message to print. If none is given the string default to "Index out of range."
    :type msg: string

    :returns:     The valid index.
    """
    # Set the default.
    if msg is None:
        msg = "Index out of range."

    # Check the type of index.
    if type(index) != int:
        msg_template = "Type of parameter 'index' should be int not {}"
        raise Error(msg_template.format(type(index)))

    # Stop if outside bounds.
    if (index < 0 or index >= len(list)):
        raise Error(msg)

    # Return if passed.
    return index


def checkCellVectors(cell_vectors):
    """
    Check that the cell vectors are of the correct dimensions and not fully linearly dependent.

    :param cell_vectors: The cell vectors to test.

    :returns: The valid cell vectors as a 3x3 numpy array.
    """
    # If this is not a numpy array, check the list and convert.
    if not isinstance(cell_vectors, numpy.ndarray):

        # If it is a list, check that it is a list of list, length 3x3
        cell_vectors = checkSequence(cell_vectors, "The 'cell_vectors' must be given as a 3x3 list or array of numbers.")

        # Check as if this was a coordinate list.
        cell_vectors = checkCoordinateList(cell_vectors, "cell_vecctors")

        # Transform to a numpy array.
        cell_vectors = numpy.array(cell_vectors)

    # Now with the cell vectors as a numpy array, check the dimension.
    if numpy.shape(cell_vectors) != (3,3):
        raise Error("The 'cell_vectors' parametes must have the shape 3x3.")

    # Check the type.
    dummy_array = numpy.array([[1.0,0.0,0.0],[0.0,1.0,0.0],[0.0,0.0,1.0]])
    if cell_vectors.dtype != dummy_array.dtype:
        raise Error("The cell_vectors elements must be floating point numbers.")

    # Check for linear dependencies.
    cell_T = numpy.transpose(cell_vectors)
    cell_determinant = numpy.linalg.det(cell_T)
    if cell_determinant < 0.00001:
        raise Error("The unit cell vectors are linearly dependent with determinant = %f."%(cell_determinant))

    # Done checking.
    return cell_vectors


def checkCoordinateList(coordinates, varname="coordinates"):
    """
    Check that the given coordinates is a valid Nx3 sequence of numbers.

    :param coordinates: The object to test. To pass the test this must be
                        an Nx3 array of floating point numbers.
    :param varname: The name of the variable. Defaults to "coordinates"
    :type varname: string
    :returns:           A valid Nx3 numpy array of numbers.
    """
    # Check that it is a sequence.
    coordinates = checkSequence(coordinates, "The %s must be given as a list of lists with dimensions Nx3" % (varname))

    # Check that its length is not zero.
    if (len(coordinates) < 1):
        raise Error("The '%s' parameter may not be an empty list." % (varname))

    # Check each coordinate.
    for coord in coordinates:

        # Check that it is a sequence.
        coord = checkSequence(coord, "The %s must be given as a list of lists with dimensions Nx3" % (varname))

        # Make sure the length of the coordinate is 3.
        if len(coord) != 3:
            raise Error("Each entry in the '%s' list must have exactly three elements." % (varname))

        # Check that each element is a floating point number.
        if not all([isinstance(c, float) for c in coord]):
            raise Error("All '%s' entries must be given as floating point numbers." % (varname))

    # Convert to a numpy array and return.
    return numpy.array(coordinates)


def checkSequence(sequence, msg="The tested object is not a sequence."):
    """
    Check that the given object is sequence.

    :param sequence: The object to test.

    :param msg: Non-default error message to print.
    :type msg: string

    :returns: The valid sequence object.
    """
    # Check that this is a sequence.
    if not ('__len__' in dir(sequence)):
        raise Error(msg)

    # Done.
    return sequence


def checkSequenceOfPositiveIntegers(sequence, msg="The tested object is not a sequence of positive integers."):
    """
    Utility function to check if a parameter is a sequence of positive integers.

    :param sequence: The sequence to check.

    :param msg: Non-default error message to print.
    :type msg: string

    :returns: The valid sequence.
    """
    # Check that it is a sequence.
    sequence = checkSequenceOf(sequence, int, msg)

    # Check that each element is a positive integer.
    for s in sequence:
        if s < 0:
            raise Error(msg)

    # Done.
    return sequence


def checkSequenceOfFloats(sequence, msg="The tested object is not a sequence of floats."):
    """
    Utility function to check if a parameter is a sequence of floating point numbers.

    :param sequence: The sequence to check.

    :param msg: Non-default error message to print.
    :type msg: string

    :returns: The valid sequence.
    """
    return checkSequenceOf(sequence, float, msg)


def checkSequenceOf(sequence, class_type, msg="The tested object is not a sequence of the correct type."):
    """
    Utility function to check if a parameter is a sequence of instances of a given type.

    :param sequence: The sequence to check.

    :param class_type: The class of which the elements in the sequence should be instances.

    :param msg: Non-default error message to print.
    :type msg: string

    :returns: The valid sequence.
    """
    # Check that it is a sequence.
    sequence = checkSequence(sequence, msg)

    # Check that its length is not zero.
    if len(sequence) == 0:
        raise Error(msg)

    # Check that each element is an instance of type KMCProcess.
    if class_type == int:
        types_group = (int, numpy.int_)
    elif class_type == float:
        types_group = (float, numpy.float_)
    elif class_type == str:
        types_group = (str, numpy.str_)
    else:
        types_group = (class_type, )

    for element in sequence:
        matched = any([isinstance(element, t) for t in types_group])
        if not matched:
            raise Error(msg)
    # Done.
    return sequence


def checkTypes(types, length):
    """
    Check that the types list is given as a list of strings with the correct
    length.

    :param types:  The object to check.

    :param length: The size the list should have.
    :type length:  int

    :returns: The checked list.
    """
    # Check that it is a list.
    if not isinstance(types, list):
        raise Error("The 'types' parameter must be given as a list of strings.")

    # Check eachg element.
    for t in types:
        if not isinstance(t, str):
            raise Error("The 'types' parameter must be given as a list of strings.")

    # Check the length.
    if len(types) != length:
        msg = ("The length of the 'types' parameter({}) " +
               "dose not match the coordinates({}).")
        msg = msg.format(len(types), length)
        raise Error(msg)

    # Done.
    return types


def checkPositiveInteger(parameter, default_parameter, parameter_name):
    """
    Utility function for checking that a parameter is a positive integer.

    :param parameter: The parameter to check.

    :param default_parameter: The value to use if the parameter value is None

    :param parameter_name: The name of the parameter to use in error messages.
    :type parameter_name: str

    :returns: The checked parameter.
    """
    # Set default.
    if parameter is None:
        parameter = default_parameter

    # The error message.
    msg = "The parameter '%s' must be given as a positive integer." % (parameter_name)
    # Check type.
    if not isinstance(parameter, int):
        raise Error(msg)

    # Check value.
    if parameter < 0:
        raise Error(msg)

    # Checked.
    return parameter


def checkPositiveFloat(parameter, default_parameter, parameter_name):
    """
    Utility function for checking that a parameter is a positive float.

    :param parameter: The parameter to check.

    :param default_parameter: The value to use if the parameter value is None

    :param parameter_name: The name of the parameter to use in error messages.
    :type parameter_name: str

    :returns: The checked parameter.
    """
    # Set default.
    if parameter is None:
        parameter = default_parameter

    # The error message.
    msg = "The parameter {} must be given as a positive float, actual: {}"
    msg = msg.format(parameter_name, parameter)

    # Check type.
    if not isinstance(parameter, float):
        raise Error(msg)

    # Check value.
    if parameter < 0.0:
        raise Error(msg)

    # Checked.
    return parameter


def checkBoolean(parameter, default_parameter, parameter_name):
    """
    Utility function for checking that a parameter is a boolean.

    :param parameter: The parameter to check.

    :param default_parameter: The value to use if the parameter value is None

    :param parameter_name: The name of the parameter to use in error messages.
    :type parameter_name: str

    :returns: The checked parameter.
    """
    # Set default.
    if parameter is None:
        parameter = default_parameter

    # The error message.
    msg = "The parameter '{}' must be given as a boolean.".format(parameter_name)
    # Check type.
    if not isinstance(parameter, bool):
        raise Error(msg)

    # Checked.
    return parameter

