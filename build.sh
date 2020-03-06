#!/bin/bash
dir="$(pwd)"

docker run \
	--rm \
	--interactive \
	--volume "$dir":/app \
	quay.io/pypa/manylinux1_x86_64 bash <<EOF
        git clone -b v2.4.3 https://github.com/pybind/pybind11.git /tmp/pybind11

        rm -fr build dist wheelhouse
        for ver in cp37-cp37m \
                   cp36-cp36m
        do
            "/opt/python/\$ver/bin/python" -m venv /tmp/pyenv
            . /tmp/pyenv/bin/activate
            pip install wheel "cmake<3.16" pytest numpy

            mkdir /tmp/pybind11-build
            cd /tmp/pybind11-build
            cmake /tmp/pybind11
            make install

            cd /app
            python setup.py sdist bdist_wheel
            deactivate
            rm -rf /tmp/pybind11-build
            rm -rf /tmp/pyenv
        done
        ls dist/*.whl | xargs -i auditwheel repair {}
        rm -fr build
EOF