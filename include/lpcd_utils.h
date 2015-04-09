#ifndef UTILS_H
#define	UTILS_H

template <typename T>
void scale(std::vector<T>& data, T from, T to) {
	T min = *std::min_element(data.begin(), data.end());
	T max = *std::max_element(data.begin(), data.end());

	for (typename std::vector<T>::iterator it = data.begin(); it != data.end(); ++it) {
		(*it) = ((to - from) * ((*it) - min) / (max - min)) + from;
	}
}

template <typename T>
void clip(T& value, T from, T to) {
	value = std::max(from, std::min(to, value));
}

template <typename T>
void clip(std::vector<T>& data, T from, T to) {
	for (typename std::vector<T>::iterator it = data.begin(); it != data.end(); ++it) {
		clip(*it, from, to);
	}
}


#endif	/* UTILS_H */

