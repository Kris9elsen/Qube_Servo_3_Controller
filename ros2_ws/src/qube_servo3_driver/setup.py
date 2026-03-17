from setuptools import setup
import os
from glob import glob

package_name = 'qube_servo3_driver'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), 
         glob('launch/*.launch.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Magne',
    maintainer_email='Magne.jacobsen04@gmail.com',
    description='ROS2 driver for Quanser Quube Servo 3',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'qube_servo3_driver = qube_servo3_driver.driver:main',
        ],
    },
)