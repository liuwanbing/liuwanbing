//
//  GGColumnCell.h
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/31.
//  Copyright © 2016年 高广. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GGColumnModel.h"
@interface GGColumnCell : UICollectionViewCell
@property (weak, nonatomic) IBOutlet UIImageView *image;
@property (weak, nonatomic) IBOutlet UILabel *name;
@property (nonatomic , strong)GGColumnModel *columnModel;
@end
