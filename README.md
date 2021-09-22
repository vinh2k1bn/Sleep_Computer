<h1> Giới thiệu </h1>
<p> &emsp; Khi biên dịch code sẽ tạo ra tiện ích command line dùng để lập lịch tự động Sleep, Hibernate máy tính. Yêu cầu Windows 10 hoặc cao hơn, ngôn ngữ hiển thị là tiếng Anh. Bạn không thể dùng phần mềm theo cách thông thường là mở phần mềm. Bạn phải truyền tham số thông qua giao diện dòng lệnh (CLI) để sử dụng. Phần mềm hiện tại không hoạt động độc lập với CLI nên nếu bạn muốn lập lịch nên thông qua file script hoặc shortcut
  </p>
<h1> Cơ chế hoạt động </h1>
 <p> &emsp; Khi bạn truyền sai tham số hoặc không hợp lệ phần mềm sẽ hiển thị thông tin về cách sử dụng. Nếu bạn truyền đúng tham số phần mềm sẽ tạm ngưng hoạt động bằng hàm Sleep() theo thời gian đặt trước (hoặc mặc định 9 giây). Sau khi hết thời gian, nếu đặt chế độ Sleep, phần mềm sẽ thực hiện Sleep máy tính bằng cách mô phỏng phím tắt Win + X > u > s, hoặc Hibernate máy tính thông qua API có sẵn của Windows.
  </p>
  <p> 
  &emsp;Vì cơ chế hoạt động ở trên, nếu bạn dùng phần mềm qua CLI sau đó tắt CLI phần mềm cũng tắt luôn và không lập lịch được, và CLI cũng không cho phép nhập lệnh tiếp theo do phải đợi phần mềm kết thúc.
  </p>
