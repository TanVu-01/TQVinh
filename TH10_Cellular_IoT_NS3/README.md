# Bài 10: Giao thức không dây tầm xa - Cellular IoT (NB-IoT, LTE-M)

**Môn:** Truyền thông IoT 2026-2027
**Sinh viên:** [Họ tên] - MSSV: [MSSV] - Lớp: [Lớp]
**Công cụ:** ns-3 (ns-3-dev), NetAnim 3.109, FlowMonitor, máy ảo Debian trên VirtualBox

---

## 1. Mục tiêu

- Xây dựng mô phỏng mạng di động LTE với lõi mạng EPC bằng ns-3, là nền tảng mạng mà các công nghệ Cellular IoT (NB-IoT, LTE-M) sử dụng.
- Quan sát cấu trúc mạng bằng NetAnim.
- Đo hiệu năng các luồng gói tin bằng FlowMonitor.

## 2. Cấu trúc mạng mô phỏng

Mô phỏng gồm 5 nút. Vai trò của từng nút được xác định dựa trên địa chỉ IP, số cổng UDP 2123 (cổng của GTP-C) và vị trí trong file NetAnim.

| Nút | Vai trò | Địa chỉ IP | Vị trí (x, y) |
|-----|---------|------------|---------------|
| 0 | PGW (cổng ra mạng ngoài) | 14.0.0.5, 7.0.0.1 | (18, 52) |
| 1 | SGW | 14.0.0.6, 13.0.0.6, 10.0.0.6 | (9, 91) |
| 2 | MME | 13.0.0.5 | (75, 59) |
| 3 | eNodeB (trạm gốc) | 10.0.0.5 | (0, 0) |
| 4 | UE (thiết bị IoT) | 7.0.0.2 | (20, 0) |

Các liên kết điểm-điểm trong lõi mạng:

| Liên kết | Hai đầu | Dải địa chỉ |
|----------|---------|-------------|
| S5 | PGW (0) - SGW (1) | 14.0.0.0 |
| S11 | MME (2) - SGW (1) | 13.0.0.0 |
| S1-U | eNodeB (3) - SGW (1) | 10.0.0.0 |

Giữa UE (4) và eNodeB (3) là kết nối vô tuyến LTE, không vẽ thành đường nối trong NetAnim.

![Sơ đồ NetAnim](images/netanim_bai10.png)

## 3. Kết quả FlowMonitor

Có 4 luồng UDP, tất cả dùng cổng 2123 (GTP-C, tín hiệu điều khiển của lõi mạng LTE).

| Flow | Hướng | Kích thước | Số gói gửi/nhận | Mất gói | Trễ |
|------|-------|-----------|-----------------|---------|-----|
| 1 | MME → SGW (13.0.0.5 → 13.0.0.6) | 164 byte | 1 / 1 | 0 | 133 ns |
| 2 | SGW → PGW (14.0.0.6 → 14.0.0.5) | 164 byte | 1 / 1 | 0 | 133 ns |
| 3 | PGW → SGW (14.0.0.5 → 14.0.0.6) | 146 byte | 1 / 1 | 0 | 118 ns |
| 4 | SGW → MME (13.0.0.6 → 13.0.0.5) | 146 byte | 1 / 1 | 0 | 118 ns |

Tổng cộng: 4 gói gửi, 4 gói nhận, **0 gói bị mất**, tổng 620 byte. Jitter bằng 0 vì mỗi luồng chỉ có một gói.

## 4. Phân tích

**Trình tự gói tin.** Theo file animation, cả 4 gói xuất hiện trong khoảng từ 20,214285 ms đến 20,214787 ms (tổng khoảng 502 ns), theo thứ tự:

1. MME → SGW (164 byte)
2. SGW → PGW (164 byte)
3. PGW → SGW (146 byte)
4. SGW → MME (146 byte)

Gói đi (164 byte) rồi gói về (146 byte) theo đường MME - SGW - PGW cho thấy đây là quá trình trao đổi bản tin điều khiển để thiết lập phiên (bearer) cho UE, nhiều khả năng là cặp bản tin yêu cầu và phản hồi tạo phiên của GTP-C.

**Về độ trễ.** Trễ 133 ns và 118 ns khớp với thời gian truyền thuần túy của gói tin trên liên kết 10 Gb/s, không có trễ lan truyền:

- 164 byte + 2 byte tiêu đề PPP = 166 byte, 166 × 8 / 10^10 ≈ 133 ns
- 146 byte + 2 byte tiêu đề PPP = 148 byte, 148 × 8 / 10^10 ≈ 118 ns

Vì vậy trễ trong lõi mạng mô phỏng gần như không đáng kể. Trễ thực tế của Cellular IoT chủ yếu nằm ở phần vô tuyến giữa UE và eNodeB.

## 5. Kết luận

- Mô phỏng thiết lập thành công mạng LTE/EPC gồm UE, eNodeB, SGW, PGW và MME.
- Quá trình trao đổi tín hiệu điều khiển trong lõi mạng diễn ra đúng trình tự, không mất gói.
- Hạn chế: kết quả chỉ ghi nhận lưu lượng điều khiển, chưa có lưu lượng dữ liệu của ứng dụng IoT giữa UE và máy chủ. Ngoài ra ns-3 dùng mô-đun LTE chung, chưa mô phỏng riêng các đặc tính của NB-IoT hay LTE-M (chế độ tiết kiệm năng lượng PSM, eDRX, băng thông hẹp).
- Hướng mở rộng: thêm ứng dụng gửi dữ liệu định kỳ từ UE (ví dụ gói nhỏ 50-100 byte mỗi 10 giây) để đo trễ và tỷ lệ mất gói của lưu lượng IoT thực sự.

## 6. Cấu trúc thư mục

```
.
├── README.md                    # Báo cáo này
├── [ten-file].cc                # Mã nguồn mô phỏng ns-3
├── mophong_bai10.xml            # File animation cho NetAnim
├── mophong_bai10_flowmon.xml    # Kết quả FlowMonitor
└── images/
    └── netanim_bai10.png        # Ảnh chụp NetAnim
```

## 7. Cách chạy lại

```bash
# Sao chép file .cc vào thư mục scratch của ns-3
cp [ten-file].cc ~/ns-3-dev/scratch/

# Chạy mô phỏng
cd ~/ns-3-dev
./ns3 run scratch/[ten-file]

# Mở animation
NetAnim   # rồi chọn file mophong_bai10.xml
```
