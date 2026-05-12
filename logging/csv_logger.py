import rclpy
from rclpy.node import Node
from diagnostic_msgs.msg import DiagnosticArray
import csv
import os
from datetime import datetime

class DiagnosticsToCSV(Node):
    def __init__(self, output_file):
        super().__init__('diagnostics_to_csv')

        self.output_file = output_file
        self.csv_file = None
        self.csv_writer = None
        self.headers_written = False
        self.start_time = None

        # Match to /diagnostics topic
        self.subscription = self.create_subscription(
            DiagnosticArray,
            '/diagnostics',
            self.callback,
            10
        )
        self.get_logger().info(f'Logging data to: {os.path.abspath(self.output_file)}')
    

    def callback(self, msg: DiagnosticArray):
        now = self.get_clock().now().nanoseconds

        if self.start_time is None:
            self.start_time = now

        elapsed_ms = (now - self.start_time) / 1_000_000 # nano to ms

        row = {'time_ms': elapsed_ms}

        for status in msg.status:
            for kv in status.values:
                row[kv.key] = kv.value

        if not self.headers_written:
            self.csv_file = open(self.output_file, 'w', newline='')
            self.csv_writer = csv.DictWriter(self.csv_file, fieldnames=row.keys())
            self.csv_writer.writeheader()
            self.headers_written = True;

        self.csv_writer.writerow(row)
        self.csv_file.flush()


    def destroy_node(self):
        if self.csv_file:
            self.csv_file.close()
            self.get_logger().info('CSV file closed.')

        super().destroy_node()


def main():
    time = datetime.now().isoformat().replace(":", "-")
    rclpy.init()
    node = DiagnosticsToCSV(time + ".csv")
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        print('\nStopped. CSV saved.')
    finally:
        node.destroy_node()

if __name__ == '__main__':
    main()
