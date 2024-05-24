import os
import time

from z3 import *
from z3.z3 import ForAll

# set_param('parallel.enable', True)
set_option(rational_to_decimal=True)


def prog_always_conjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                s.add(sig0(j) == data_0[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                s.add(sig1(j) == data_1[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if not entryFound:

            print()

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            Implies(
                And(v >= timestamps0[0], v <= timestamps0[-1]),
                And(z3Interpolate(c_flow, v) < 2),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            print("unsat in segment", i)

            # terminate after unsat
            return

        elif i <= segCount:

            print("sat in segment", i)

        s.reset()


def prog_always_disjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                s.add(sig0(j) == data_0[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                s.add(sig1(j) == data_1[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if not entryFound:

            print()

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            Implies(
                And(v >= timestamps0[0], v <= timestamps0[-1]),
                And(z3Interpolate(c_flow, v) == 0),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
            print("unsat in segment", i)

            # terminate after unsat
            return

        elif i <= segCount:

            print("sat in segment", i)

        s.reset()


def prog_eventually_conjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        # for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

        #     if j >= 0 and j < len(data_0):

        #         timestamps0.append(j)

        #         s.add(sig0(j) == data_0[j][1])

        #         if j > (i * segDur):

        #             entryFound = True

        #         if j == sigDur:

        #             entryFound = False

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig0(j) == data_0[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig0(j) == -999)

        for j in range(len(data_0), len(data_0) + eps):

            s.add(sig0(j) == -999)

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        # for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

        #     if j >= 0 and j < len(data_1):

        #         timestamps1.append(j)

        #         s.add(sig1(j) == data_1[j][1])

        #         if j > (i * segDur):

        #             entryFound = True

        #         if j == sigDur:

        #             entryFound = False

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig1(j) == data_1[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig1(j) == -999)

        for j in range(len(data_1), len(data_1) + eps):

            s.add(sig1(j) == -999)

        # if not entryFound:

        #     print()

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c0(i) == (min(len(data_0), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c1(i) == (min(len(data_1), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            Implies(
                And(v >= timestamps0[0], v <= timestamps0[-1]),
                And(z3Interpolate(c_flow, v) >= 2),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
            print("sat in segment", i)

            # terminate after sat
            return

        elif i <= segCount:

            print("unsat in segment", i)

        s.reset()


def prog_eventually_disjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                s.add(sig0(j) == data_0[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                s.add(sig1(j) == data_1[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if not entryFound:

            print()

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            Implies(
                And(v >= timestamps0[0], v <= timestamps0[-1]),
                And(z3Interpolate(c_flow, v) >= 1),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            print("sat in segment", i)

            # terminate after sat
            return

        elif i <= segCount:

            print("unsat in segment", i)

        s.reset()


def prog_until_old(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                s.add(sig0(j) == data_0[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                s.add(sig1(j) == data_1[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if not entryFound:

            print()

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check (until)
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        u = Int("u")
        s.add(And(u >= timestamps0[0], u <= v))

        s.add(
            ForAll(
                u,
                Implies(
                    And(
                        u >= timestamps0[0],
                        u <= v,
                        v >= timestamps0[0],
                        v <= timestamps0[-1],
                    ),
                    And(z3Interpolate(c_flow, u) >= 1, z3Interpolate(c_flow, v) == 2),
                ),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            print("sat in segment", i)

        elif i <= segCount:

            for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

                if j >= 0 and j < len(data_0) and data_0[j][1] == 0:

                    print("unsat in segment", i)
                    return

            print("unknown in segment", i)

        s.reset()


def prog_eventually_conjunction(eps, segCount, data_0, data_1, a, b):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig0(j) == data_0[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig0(j) == -999)

        for j in range(len(data_0), len(data_0) + eps):

            s.add(sig0(j) == -999)

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig1(j) == data_1[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig1(j) == -999)

        for j in range(len(data_1), len(data_1) + eps):

            s.add(sig1(j) == -999)

        # if not entryFound:

        #     print()

        i += 1

        # force terminate after one loop
        entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c0(i) == (min(len(data_0), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c1(i) == (min(len(data_1), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= a, v <= b))

        s.add(
            Implies(
                And(v >= a, v <= b),
                And(z3Interpolate(c_flow, v) >= 2),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
            print("sat in segment", i)

            # terminate after sat
            return

        elif i <= segCount:

            print("unsat in segment", i)

        s.reset()


def prog_until(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig0(j) == data_0[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig0(j) == -999)

        for j in range(len(data_0), len(data_0) + eps):

            s.add(sig0(j) == -999)

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig1(j) == data_1[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig1(j) == -999)

        for j in range(len(data_1), len(data_1) + eps):

            s.add(sig1(j) == -999)

        # if not entryFound:

        #     print()

        i += 1

        # force terminate after one loop
        entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c0(i) == (min(len(data_0), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c1(i) == (min(len(data_1), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        # s.add(
        #     Implies(
        #         And(v >= timestamps0[0], v <= timestamps0[-1]),
        #         And(z3Interpolate(c_flow, v) >= 2),
        #     )
        # )

        u = Int("u")
        s.add(And(u >= timestamps0[0], u <= v))

        # s.add(
        #     ForAll(
        #         u,
        #         Implies(
        #             And(u >= timestamps0[0], u <= v),
        #             (z3Interpolate(sig0, u) >= 1),
        #         ),
        #     )
        # )

        s.add(
            ForAll(
                u,
                Implies(
                    And(
                        u >= timestamps0[0],
                        u <= v,
                        v >= timestamps0[0],
                        v <= timestamps0[-1],
                    ),
                    And(z3Interpolate(sig0, u) >= 1, z3Interpolate(c_flow, v) >= 2),
                ),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
            print("sat in segment", i)

        elif i <= segCount:

            for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

                if j >= 0 and j < len(data_0) and data_0[j][1] == 0:

                    print("unsat in segment", i)
                    return

            print("unknown in segment", i)

        s.reset()


def prog_always_implies_eventually(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig0(j) == data_0[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig0(j) == -999)

        for j in range(len(data_0), len(data_0) + eps):

            s.add(sig0(j) == -999)

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig1(j) == data_1[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig1(j) == -999)

        for j in range(len(data_1), len(data_1) + eps):

            s.add(sig1(j) == -999)

        # if not entryFound:

        #     print()

        i += 1

        # force terminate after one loop
        entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c0(i) == (min(len(data_0), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c1(i) == (min(len(data_1), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        u = Int("u")
        v = Int("v")

        s.add(And(v >= u, v <= timestamps0[-1]))
        s.add(And(u >= timestamps0[0], u <= timestamps0[-1]))

        # ALWAYS
        s.add(
            ForAll(
                u,
                Implies(
                    And(
                        u >= timestamps0[0],
                        u <= timestamps0[-1],
                        v >= u,
                        v <= timestamps0[-1],
                        z3Interpolate(sig0, u) >= 1,
                    ),
                    z3Interpolate(sig1, v) >= 1,
                ),
            )
        )

        # W/O ALWAYS
        # s.add(
        #     Implies(
        #         And(
        #             u >= timestamps0[0],
        #             u <= timestamps0[-1],
        #             v >= u,
        #             v <= timestamps0[-1],
        #         ),
        #         And(z3Interpolate(sig0, u) >= 1, z3Interpolate(sig1, v) >= 1),
        #     )
        # )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
            print("sat in segment", i)

        elif i <= segCount:

            print("unsat in segment", i)

        s.reset()


def prog_always_implies_eventually(eps, segCount, data_0, data_1, a, b):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig0(j) == data_0[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig0(j) == -999)

        for j in range(len(data_0), len(data_0) + eps):

            s.add(sig0(j) == -999)

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        for j in range(0, len(data_0)):

            s.add(sig1(j) == data_1[j][1])

        # band aid fix for some edge cases
        for j in range(-eps, 0):

            s.add(sig1(j) == -999)

        for j in range(len(data_1), len(data_1) + eps):

            s.add(sig1(j) == -999)

        # if not entryFound:

        #     print()

        i += 1

        # force terminate after one loop
        entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c0(i) == (min(len(data_0), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or(
                        [
                            c1(i) == (min(len(data_1), (max(0, ((i - eps) + j)))))
                            for j in range(2 * int(eps) + 1)
                        ]
                    )
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        u = Int("u")
        v = Int("v")

        s.add(And(u >= timestamps0[0], u <= timestamps0[-1]))
        s.add(
            And(v >= u + a, v <= u + b)
        )  # This should probably be min(u + b, timestamps0[-1]) to handle some edge cases

        # ALWAYS
        s.add(
            ForAll(
                u,
                Implies(
                    And(
                        u >= timestamps0[0],
                        u <= timestamps0[-1],
                        v >= u + a,
                        v
                        <= u
                        + b,  # This should probably be min(u + b, timestamps0[-1]) to handle some edge cases
                        z3Interpolate(sig0, u) >= 1,
                    ),
                    z3Interpolate(sig1, v) >= 1,
                ),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
            print("sat in segment", i)

        elif i <= segCount:

            print("unsat in segment", i)

        s.reset()


def z3Interpolate(f, p):

    # return f(ToInt(p)) + ((f(ToInt(p) + 1) - f(ToInt(p))) * (p - ToInt(p)))
    return f(p)


def z3Abs(x):

    return If(x >= 0, x, -x)


def z3SqDist3d(x1, x2, y1, y2, z1, z2):

    return ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)) + ((z2 - z1) * (z2 - z1))


def z3SqDist2d(x1, x2, y1, y2):

    return ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))


def z3SqDist1d(x1, x2):

    return z3Abs(x2 - x1)


def getDataTest(agent_ID):

    # file = open("data/{}_{}_{}_{}_{}.txt".format(d, eps, delim, edges, agent_ID))
    file = open("test{}.txt".format(agent_ID))
    line = file.readline()

    data = []

    while line:

        # param = line.split("\t")
        param = line.split(" ")

        values = []

        for i in range(2):

            values.append(float(param[i].strip()))

        data.append(values)

        line = file.readline()

    file.close()

    # print(data)

    return data


def getData(d, setID):

    # file = open("data/{}_{}_{}_{}_{}.txt".format(d, eps, delim, edges, agent_ID))
    file = open(
        "{}\\dataNew\\{}_{}.txt".format(
            os.path.dirname(os.path.abspath(__file__)), d, setID
        )
    )
    line = file.readline()

    data = []

    while line:

        # param = line.split("\t")
        param = line.split(" ")

        values = []

        for i in range(2):

            values.append(float(param[i].strip()))

        data.append(values)

        line = file.readline()

    file.close()

    # print(data)

    return data


def main():

    # set repeat count for confidence interval
    repeat = 1
    eps = 1
    # read data from files
    d = 4
    id = 0
    data_0 = getData(d, id)
    data_1 = getData(d, id + 100)

    for i in range(len(data_0)):
        if data_0[i][1] > 0:
            data_0[i][1] = 1.0
        else:
            data_0[i][1] = 0.0
        if data_1[i][1] > 0:
            data_1[i][1] = 1.0
        else:
            data_1[i][1] = 0.0

    data_0.append([float(len(data_0)), data_0[len(data_0) - 1][1]])
    data_1.append([float(len(data_1)), data_1[len(data_1) - 1][1]])

    total_time = 0
    for i in range(repeat):
        start = time.time()
        # prog_always_conjunction(eps, 1, data_0, data_1)
        # prog_always_disjunction(eps, 1, data_0, data_1)
        # prog_eventually_conjunction(eps, 2, data_0, data_1)
        # prog_eventually_disjunction(eps, 1, data_0, data_1)
        # prog_until(eps, 1, data_0, data_1)
        # prog_eventually_conjunction(eps, 1, data_0, data_1, 4, 7)
        # prog_always_implies_eventually(eps, 1, data_0, data_1)
        prog_always_implies_eventually(eps, 1, data_0, data_1, 4, 7)
        end = time.time()
        # print("\nTime elapsed :", (end - start), "seconds")
        dur = end - start
        # print(i, "\t:\t", dur)
        total_time += dur
    print("Average :\t", total_time / repeat)


if __name__ == "__main__":

    main()
    pass
