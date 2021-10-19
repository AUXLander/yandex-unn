#include "w2t5.hpp"
#include <vector>
#include <stack>
#include <algorithm>
#include <fstream>

struct Vertex;

template<class T>
class PlaceFixedList
{
	friend class Vertex;

	struct Node;

	const size_t cap;
	size_t sz;

	char* pAllocationB;

	Node* pAllocation;
	Node* pFirst;
	Node* pLast;

	struct Node
	{
		PlaceFixedList& list;

		T* value;

		bool isVisible;

		Node* prev;
		Node* next;

		Node(PlaceFixedList& list, T* value, const bool isVisible = false, Node* prev = nullptr)
			: list(list), value(value), isVisible(isVisible), prev(prev), next(nullptr)
		{
			if (prev != nullptr)
			{
				prev->next = this;
			}
		}

		~Node() {}

		void hide()
		{
			isVisible = false;

			if (prev != nullptr)
			{
				prev->next = next;
			}
			else
			{
				list.pFirst = next;
			}

			if (next != nullptr)
			{
				next->prev = prev;
			}
			else
			{
				list.pLast = prev;
			}

			--list.sz;
		}

		void show()
		{
			isVisible = true;

			if (prev != nullptr)
			{
				prev->next = this;
			}
			else
			{
				list.pFirst = this;
			}

			if (next != nullptr)
			{
				next->prev = this;
			}
			else
			{
				list.pLast = this;
			}

			++list.sz;
		}
	};

public:
	explicit PlaceFixedList(std::vector<T>& container)
		: cap(container.size()), sz(0),
		  pAllocationB(nullptr), pAllocation(nullptr), pFirst(nullptr), pLast(nullptr)
	{
		pAllocationB = new char[sizeof(Node) * cap];

		pAllocation = reinterpret_cast<Node*>(pAllocationB);

		size_t index = 0;

		Node* pPrevious = nullptr;

		if (cap > 0)
		{
			pFirst = pPrevious = new(pAllocation) Node(*this, container.data() + index, false, nullptr);

			for (index = 1; index < cap; ++index)
			{
				pPrevious = new(pAllocation + index) Node(*this, container.data() + index, false, pPrevious);
			}

			if (cap > 0)
			{
				pLast = pPrevious;
			}
		}

		sz = index;
	}

	Node* first()
	{
		return pFirst;
	}

	Node* last()
	{
		return pLast;
	}

	size_t size() const
	{
		return sz;
	}

	size_t capacity() const
	{
		return cap;
	}

	~PlaceFixedList()
	{
		delete[] pAllocationB;
	}
};

struct Point
{
	int xx;
	int yy;

	Point(int x, int y) : xx(x), yy(y) {};
};


struct Vertex
{
	Point point;
	
	bool isVisited;
	
	PlaceFixedList<Vertex>::Node* pNode;

	explicit Vertex(const Point& point, const bool visit = false, PlaceFixedList<Vertex>::Node* pNode = nullptr)
		: point(point), isVisited(visit), pNode(pNode) {}

	void reset()
	{
		isVisited = false;
	}

	void setNode(PlaceFixedList<Vertex>::Node* pNode)
	{
		this->pNode = pNode;
	}

	static int distance(const Vertex& first, const Vertex& second)
	{
		return std::abs(first.point.xx - second.point.xx);
	}

	void visit()
	{
		if (pNode != nullptr)
		{
			pNode->hide();
		}

		isVisited = true;
	}

	void unvisit()
	{
		if (pNode != nullptr)
		{
			pNode->show();
		}

		isVisited = false;
	}

	Vertex& operator=(const Vertex& other)
	{
		point = other.point;
		isVisited = other.isVisited;

		return *this;
	}
};

class VertexBuffer
{
	size_t locked_count = 0;

	PlaceFixedList<Vertex> cache;

public:
	explicit VertexBuffer(std::vector<Vertex>& map) : cache(map)
	{
		auto* pNode = cache.first();

		if (pNode != nullptr)
		{
			do
			{
				pNode->value->setNode(pNode);
				pNode = pNode->next;
			} 
			while ((pNode != nullptr) && (pNode->next != nullptr));

			if (pNode != nullptr)
			{
				pNode->value->setNode(pNode);
			}
		}
	}

	void lock(Vertex& element)
	{
		element.visit();
		
		++locked_count;
	}

	void unlock(Vertex& element)
	{
		element.unvisit();
		
		--locked_count;
	}

	Vertex* next(const size_t offset)
	{
		size_t sub_offset = 0;

		auto* pNode = cache.first();

		if (pNode != nullptr)
		{
			for (; sub_offset < offset; ++sub_offset)
			{
				if (pNode->next == nullptr)
				{
					if (pNode == pNode->list.last())
					{
						pNode = pNode->list.first();
					}
					else
					{
						return nullptr;
					}
				}
				else
				{
					pNode = pNode->next;
				}
			}
			
			return pNode->value;
		}

		return nullptr;
	}

	size_t avaible() const
	{
		return cache.capacity() - locked_count;
	}
};

constexpr int invalid_time_value = 3'000'000;

int research(VertexBuffer& container, Vertex& element, const int time)
{
	container.lock(element);

	int min_time = invalid_time_value;
	const size_t avaible_count = container.avaible();

	for (size_t offset = 0; offset < avaible_count; ++offset)
	{
		auto* pNext = container.next(offset);

		if (pNext != nullptr)
		{
			auto& next = *pNext;

			auto distance = Vertex::distance(next, element);

			if (next.point.yy - distance - time >= 0)
			{
				const int new_time = research(container, next, time + distance);

				if (new_time >= 0)
				{
					min_time = std::min(min_time, new_time);
				}
			}
			else
			{
				container.unlock(element);

				return invalid_time_value;
			}
		}
		else
		{
			min_time = time;

			break;
		}
	}

	if (avaible_count == 0)
	{
		min_time = time;
	}

	container.unlock(element);

	return min_time;
}

void W2T5::main(std::istream& input, std::ostream& output)
{
	std::vector<Vertex> map;
	std::vector<Point> points;

	int N;
	int x, y;

	input >> N;

	points.reserve(N);

	for (int i = 0; i < N; i++)
	{
		input >> x;
		input >> y;

		points.emplace_back(x, y);
	}

	if (N == 1)
	{
		output << 0;

		return;
	}

	map.reserve(points.size());

	for (const auto& point : points)
	{
		map.emplace_back(point);
	}

	std::sort(map.begin(), map.end(), [](const Vertex& lhs, const Vertex& rhs) { return lhs.point.yy < rhs.point.yy; });

	VertexBuffer container(map);

	int min_time = invalid_time_value;

	for (auto& vertex : map)
	{
		int time = research(container, vertex, 0);

		if (time >= 0)
		{
			min_time = std::min(time, min_time);
		}
	}

	if (min_time == invalid_time_value)
	{
		output << "No solution";
	}
	else
	{
		output << min_time;
	}
}

void W2T5::test(Test* const reference)
{
	if (reference != nullptr)
	{
		std::fstream file_input("input.txt");

		std::string inp1000 = "1000 0 1000 1 999 2 998 3 997 4 996 5 995 6 994 7 993 8 992 9 991 10 990 11 989 12 988 13 987 14 986 15 985 16 984 17 983 18 982 19 981 20 980 21 979 22 978 23 977 24 976 25 975 26 974 27 973 28 972 29 971 30 970 31 969 32 968 33 967 34 966 35 965 36 964 37 963 38 962 39 961 40 960 41 959 42 958 43 957 44 956 45 955 46 954 47 953 48 952 49 951 50 950 51 949 52 948 53 947 54 946 55 945 56 944 57 943 58 942 59 941 60 940 61 939 62 938 63 937 64 936 65 935 66 934 67 933 68 932 69 931 70 930 71 929 72 928 73 927 74 926 75 925 76 924 77 923 78 922 79 921 80 920 81 919 82 918 83 917 84 916 85 915 86 914 87 913 88 912 89 911 90 910 91 909 92 908 93 907 94 906 95 905 96 904 97 903 98 902 99 901 100 900 101 899 102 898 103 897 104 896 105 895 106 894 107 893 108 892 109 891 110 890 111 889 112 888 113 887 114 886 115 885 116 884 117 883 118 882 119 881 120 880 121 879 122 878 123 877 124 876 125 875 126 874 127 873 128 872 129 871 130 870 131 869 132 868 133 867 134 866 135 865 136 864 137 863 138 862 139 861 140 860 141 859 142 858 143 857 144 856 145 855 146 854 147 853 148 852 149 851 150 850 151 849 152 848 153 847 154 846 155 845 156 844 157 843 158 842 159 841 160 840 161 839 162 838 163 837 164 836 165 835 166 834 167 833 168 832 169 831 170 830 171 829 172 828 173 827 174 826 175 825 176 824 177 823 178 822 179 821 180 820 181 819 182 818 183 817 184 816 185 815 186 814 187 813 188 812 189 811 190 810 191 809 192 808 193 807 194 806 195 805 196 804 197 803 198 802 199 801 200 800 201 799 202 798 203 797 204 796 205 795 206 794 207 793 208 792 209 791 210 790 211 789 212 788 213 787 214 786 215 785 216 784 217 783 218 782 219 781 220 780 221 779 222 778 223 777 224 776 225 775 226 774 227 773 228 772 229 771 230 770 231 769 232 768 233 767 234 766 235 765 236 764 237 763 238 762 239 761 240 760 241 759 242 758 243 757 244 756 245 755 246 754 247 753 248 752 249 751 250 750 251 749 252 748 253 747 254 746 255 745 256 744 257 743 258 742 259 741 260 740 261 739 262 738 263 737 264 736 265 735 266 734 267 733 268 732 269 731 270 730 271 729 272 728 273 727 274 726 275 725 276 724 277 723 278 722 279 721 280 720 281 719 282 718 283 717 284 716 285 715 286 714 287 713 288 712 289 711 290 710 291 709 292 708 293 707 294 706 295 705 296 704 297 703 298 702 299 701 300 700 301 699 302 698 303 697 304 696 305 695 306 694 307 693 308 692 309 691 310 690 311 689 312 688 313 687 314 686 315 685 316 684 317 683 318 682 319 681 320 680 321 679 322 678 323 677 324 676 325 675 326 674 327 673 328 672 329 671 330 670 331 669 332 668 333 667 334 666 335 665 336 664 337 663 338 662 339 661 340 660 341 659 342 658 343 657 344 656 345 655 346 654 347 653 348 652 349 651 350 650 351 649 352 648 353 647 354 646 355 645 356 644 357 643 358 642 359 641 360 640 361 639 362 638 363 637 364 636 365 635 366 634 367 633 368 632 369 631 370 630 371 629 372 628 373 627 374 626 375 625 376 624 377 623 378 622 379 621 380 620 381 619 382 618 383 617 384 616 385 615 386 614 387 613 388 612 389 611 390 610 391 609 392 608 393 607 394 606 395 605 396 604 397 603 398 602 399 601 400 600 401 599 402 598 403 597 404 596 405 595 406 594 407 593 408 592 409 591 410 590 411 589 412 588 413 587 414 586 415 585 416 584 417 583 418 582 419 581 420 580 421 579 422 578 423 577 424 576 425 575 426 574 427 573 428 572 429 571 430 570 431 569 432 568 433 567 434 566 435 565 436 564 437 563 438 562 439 561 440 560 441 559 442 558 443 557 444 556 445 555 446 554 447 553 448 552 449 551 450 550 451 549 452 548 453 547 454 546 455 545 456 544 457 543 458 542 459 541 460 540 461 539 462 538 463 537 464 536 465 535 466 534 467 533 468 532 469 531 470 530 471 529 472 528 473 527 474 526 475 525 476 524 477 523 478 522 479 521 480 520 481 519 482 518 483 517 484 516 485 515 486 514 487 513 488 512 489 511 490 510 491 509 492 508 493 507 494 506 495 505 496 504 497 503 498 502 499 501 500 500 501 499 502 498 503 497 504 496 505 495 506 494 507 493 508 492 509 491 510 490 511 489 512 488 513 487 514 486 515 485 516 484 517 483 518 482 519 481 520 480 521 479 522 478 523 477 524 476 525 475 526 474 527 473 528 472 529 471 530 470 531 469 532 468 533 467 534 466 535 465 536 464 537 463 538 462 539 461 540 460 541 459 542 458 543 457 544 456 545 455 546 454 547 453 548 452 549 451 550 450 551 449 552 448 553 447 554 446 555 445 556 444 557 443 558 442 559 441 560 440 561 439 562 438 563 437 564 436 565 435 566 434 567 433 568 432 569 431 570 430 571 429 572 428 573 427 574 426 575 425 576 424 577 423 578 422 579 421 580 420 581 419 582 418 583 417 584 416 585 415 586 414 587 413 588 412 589 411 590 410 591 409 592 408 593 407 594 406 595 405 596 404 597 403 598 402 599 401 600 400 601 399 602 398 603 397 604 396 605 395 606 394 607 393 608 392 609 391 610 390 611 389 612 388 613 387 614 386 615 385 616 384 617 383 618 382 619 381 620 380 621 379 622 378 623 377 624 376 625 375 626 374 627 373 628 372 629 371 630 370 631 369 632 368 633 367 634 366 635 365 636 364 637 363 638 362 639 361 640 360 641 359 642 358 643 357 644 356 645 355 646 354 647 353 648 352 649 351 650 350 651 349 652 348 653 347 654 346 655 345 656 344 657 343 658 342 659 341 660 340 661 339 662 338 663 337 664 336 665 335 666 334 667 333 668 332 669 331 670 330 671 329 672 328 673 327 674 326 675 325 676 324 677 323 678 322 679 321 680 320 681 319 682 318 683 317 684 316 685 315 686 314 687 313 688 312 689 311 690 310 691 309 692 308 693 307 694 306 695 305 696 304 697 303 698 302 699 301 700 300 701 299 702 298 703 297 704 296 705 295 706 294 707 293 708 292 709 291 710 290 711 289 712 288 713 287 714 286 715 285 716 284 717 283 718 282 719 281 720 280 721 279 722 278 723 277 724 276 725 275 726 274 727 273 728 272 729 271 730 270 731 269 732 268 733 267 734 266 735 265 736 264 737 263 738 262 739 261 740 260 741 259 742 258 743 257 744 256 745 255 746 254 747 253 748 252 749 251 750 250 751 249 752 248 753 247 754 246 755 245 756 244 757 243 758 242 759 241 760 240 761 239 762 238 763 237 764 236 765 235 766 234 767 233 768 232 769 231 770 230 771 229 772 228 773 227 774 226 775 225 776 224 777 223 778 222 779 221 780 220 781 219 782 218 783 217 784 216 785 215 786 214 787 213 788 212 789 211 790 210 791 209 792 208 793 207 794 206 795 205 796 204 797 203 798 202 799 201 800 200 801 199 802 198 803 197 804 196 805 195 806 194 807 193 808 192 809 191 810 190 811 189 812 188 813 187 814 186 815 185 816 184 817 183 818 182 819 181 820 180 821 179 822 178 823 177 824 176 825 175 826 174 827 173 828 172 829 171 830 170 831 169 832 168 833 167 834 166 835 165 836 164 837 163 838 162 839 161 840 160 841 159 842 158 843 157 844 156 845 155 846 154 847 153 848 152 849 151 850 150 851 149 852 148 853 147 854 146 855 145 856 144 857 143 858 142 859 141 860 140 861 139 862 138 863 137 864 136 865 135 866 134 867 133 868 132 869 131 870 130 871 129 872 128 873 127 874 126 875 125 876 124 877 123 878 122 879 121 880 120 881 119 882 118 883 117 884 116 885 115 886 114 887 113 888 112 889 111 890 110 891 109 892 108 893 107 894 106 895 105 896 104 897 103 898 102 899 101 900 100 901 99 902 98 903 97 904 96 905 95 906 94 907 93 908 92 909 91 910 90 911 89 912 88 913 87 914 86 915 85 916 84 917 83 918 82 919 81 920 80 921 79 922 78 923 77 924 76 925 75 926 74 927 73 928 72 929 71 930 70 931 69 932 68 933 67 934 66 935 65 936 64 937 63 938 62 939 61 940 60 941 59 942 58 943 57 944 56 945 55 946 54 947 53 948 52 949 51 950 50 951 49 952 48 953 47 954 46 955 45 956 44 957 43 958 42 959 41 960 40 961 39 962 38 963 37 964 36 965 35 966 34 967 33 968 32 969 31 970 30 971 29 972 28 973 27 974 26 975 25 976 24 977 23 978 22 979 21 980 20 981 19 982 18 983 17 984 16 985 15 986 14 987 13 988 12 989 11 990 10 991 9 992 8 993 7 994 6 995 5 996 4 997 3 998 2 999 1";

		reference->open(*this).input(inp1000).expect("999");

		for (int i = 0; i <= 1'000'000; ++i)
		{
			std::string input = "2 ";

			input += std::to_string(1) + ' ' + std::to_string(1) + ' ';

			input += std::to_string(2147483 - i) + ' ' + std::to_string(2147483 - i) + ' ';

			const bool result = reference->open(*this).input(input).expect(std::to_string(2147483 - i - 1));

			if (result == false)
			{
				input = input;
			}
		}
	}
};